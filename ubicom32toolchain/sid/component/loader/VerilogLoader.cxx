// VerilogLoader.cxx - object file loader component.  -*- C++ -*-
//
// by Toshiyasu Morita
//
// Hacked from compLoader.cxx
//
// This loader loads files in the Verilog $readmemh format as described in:
//
// "IEEE Standard Verilog Hardware Description Language" IEEE Std 1364-2001, Section 17.2.8
//

// Copyright (C) 1999, 2000, 2003, 2004, 2005 Red Hat.
// This file is part of SID and is licensed under the GPL.
// See the file COPYING.SID for conditions for redistribution.

#include <sidcomp.h>
#include <sidso.h>
#include <sidcomputil.h>
#include <sidattrutil.h>
#include <sidpinutil.h>
#include <sidbusutil.h>
#include <sidcpuutil.h>
#include <sidtypes.h>
#include <sidmiscutil.h>

#include <vector>
#include <string>
#include <cstdlib>
#include <ctime>
#include <cerrno>
#include <unistd.h>
#include <fstream>

using std::vector;
using std::string;
using std::ostream;
using std::istream;
using std::ios;
using std::endl;
using std::cout;
using std::ifstream;
using std::cerr;

using sid::component;
using sid::bus;
using sid::host_int_1;
using sid::little_int_1;
using sid::host_int_4;
using sid::host_int_8;
using sid::component_library;
using sid::COMPONENT_LIBRARY_MAGIC;

using sidutil::fixed_bus_map_component;
using sidutil::fixed_attribute_map_component;
using sidutil::fixed_pin_map_component;
using sidutil::fixed_accessor_map_component;
using sidutil::no_relation_component;
using sidutil::configurable_component;
using sidutil::output_pin;
using sidutil::callback_pin;
using sidutil::string2stream;
using sidutil::stream2string;
using sidutil::make_attribute;
using sidutil::make_numeric_attribute;
using sidutil::parse_attribute;
using sidutil::std_error_string;

// ----------------------------------------------------------------------------

// A bus for allowing the loader to perform random checks against reads and writes
// to memory. For example writing to a code area. Default implementation

class loader_probe_bus: public sidutil::passthrough_bus
{
	public:
		loader_probe_bus(sid::bus **t, output_pin *p) :
		sidutil::passthrough_bus(t),
		write_to_code_address_pin(p),
		section_table(0)
		{
			assert(t);
		}

		~loader_probe_bus() throw() {}

		void set_section_table(const struct TextSection *s)
		{
			section_table = s;
		}

		void probe_address(sid::host_int_4 addr)
		{
		}

		// Some macros to make manufacturing of the cartesian-product
		// calls simpler.
#define SID_GB_WRITE(dtype) \
		sid::bus::status write(sid::host_int_4 addr, dtype data) throw ()\
		{ \
			if (LIKELY(*target)) { \
		                probe_address (addr); \
               			return (*target)->write(addr, data); \
			} else \
				return sid::bus::unpermitted; \
		}

		SID_GB_WRITE(sid::little_int_1)
		SID_GB_WRITE(sid::big_int_1)
		SID_GB_WRITE(sid::little_int_2)
		SID_GB_WRITE(sid::big_int_2)
		SID_GB_WRITE(sid::little_int_4)
		SID_GB_WRITE(sid::big_int_4)
		SID_GB_WRITE(sid::little_int_8)
		SID_GB_WRITE(sid::big_int_8)

#undef SID_GB_WRITE

		output_pin *write_to_code_address_pin;
		const struct TextSection *section_table;
};

class generic_verilog_loader: public virtual component,
		      protected fixed_bus_map_component,
		      protected virtual fixed_attribute_map_component,
		      protected virtual fixed_pin_map_component,
		      protected fixed_accessor_map_component,
		      protected no_relation_component,
		      protected virtual configurable_component
{
	private:
		callback_pin<generic_verilog_loader> doit_pin;

	protected:
		// entry point address
		output_pin start_pc_pin;

		// This is unused, but required because the conf file uses it.
		output_pin endian_pin;

		// Provide address of write attempt to code section
		output_pin write_to_code_address_pin;

		// Signal this if something went wrong.
		output_pin error_pin;

		// Attribute settings
		bool verbose_p;

		// loadable file names
		string load_file;

		// accessors
		bus *load_accessor_insn;
		bus *load_accessor_data;

		loader_probe_bus probe_upstream;
		bus *probe_downstream;
		callback_pin<generic_verilog_loader> probe_pin;

		void handle_probe_pin(sid::host_int_4 v)
		{
			probe_upstream.probe_address (v);
		}

		// The load pin was triggered.
		virtual void load_it(host_int_4) = 0;

		// state control
		friend ostream& operator << (ostream& o, const generic_verilog_loader& it);
		friend istream& operator >> (istream& i, generic_verilog_loader& it);
		string save_state()
		{
			return make_attribute(*this);
		}
		sid::component::status restore_state(const string& state)
		{
			return parse_attribute(state, *this);
		}
		virtual void configure(const string &config);

	public:
		generic_verilog_loader();
		~generic_verilog_loader() throw() { }
};

generic_verilog_loader::generic_verilog_loader()
	: doit_pin(this, &generic_verilog_loader::load_it),
	verbose_p(false),
	load_file("/dev/null"),
	load_accessor_insn(0),
	load_accessor_data(0),
	probe_upstream(& probe_downstream, & this->write_to_code_address_pin),
	probe_downstream(0),
	probe_pin(this, & generic_verilog_loader::handle_probe_pin)
{
	add_pin("load!", &this->doit_pin);
	add_pin("start-pc-set", &this->start_pc_pin);
	add_pin("endian-set", & this->endian_pin);
	add_pin("error", & this->error_pin);
	add_pin("write-to-code-address", & this->write_to_code_address_pin);
	add_accessor("load-accessor-insn", & this->load_accessor_insn);
	add_accessor("load-accessor-data", & this->load_accessor_data);
	add_attribute("file", & this->load_file, "setting");
	add_bus("probe-upstream", & this->probe_upstream);
	add_accessor ("probe-downstream", & this->probe_downstream);
	add_pin("probe", & this->probe_pin);
	add_attribute("verbose?", & this->verbose_p, "setting");
	add_attribute_virtual ("state-snapshot", this, &generic_verilog_loader::save_state, &generic_verilog_loader::restore_state);
}

void generic_verilog_loader::configure (const string &config)
{
	// Call up to the base class first
	configurable_component::configure (config);

	// Now handle relevent configuration for us.
	if (config.size() < 12)
		return;

	if (config.substr(0, 8) == "verbose=") {
		verbose_p = (config.substr (8) == "true");
		return;
	}
}

ostream& operator << (ostream& out, const generic_verilog_loader& it)
{
	out << "loader-state " << string2stream(it.load_file) << " " << it.verbose_p;

	return out;
}

istream& operator >> (istream& in, generic_verilog_loader& it)
{
	string coding;
	in >> coding;
	if (coding == "loader-state") {
		in >> stream2string(it.load_file) >> it.verbose_p;
	} else {
		in.setstate(ios::badbit);
	}

	return in;
}

// ----------------------------------------------------------------------------

class verilog_loader: public generic_verilog_loader
{
	public:
		verilog_loader ()
			: generic_verilog_loader (),
			symbol_table (0),
			current_function (""),
			check_function_pin (this, &verilog_loader::check_function_pin_handler)
		{
			add_pin ("function?", &check_function_pin);
			add_attribute ("current-function", & current_function);
		}
		~verilog_loader () throw () {}

	private:
		// static pointer to active instance (XXX: concurrency?)
		static verilog_loader *freeloader;

		static int verbose_function(char* s);

		// stream for current file  
		ifstream* file;

		/* load_it() is triggered by the load pin, and starts the loading process */

		void load_it (host_int_4);

	protected:
		callback_pin<verilog_loader> check_function_pin;
		void check_function_pin_handler(host_int_4 addr);

		const struct Symbol *symbol_table;
		string current_function;
};

// static variable
verilog_loader* verilog_loader::freeloader = 0;

#define BUFFER_SIZE 4096

void verilog_loader::load_it (host_int_4)
{
	bus::status retval;
	char line[BUFFER_SIZE], *input;
	int c, i, digits, address_flag, bytes_num;
	host_int_4 value, address;

	if (this->load_accessor_insn == 0 || this->load_accessor_data == 0) {
		cerr << "VerilogLoader: error - target accessors not configured!" << endl;
		this->error_pin.drive (0);
		return;
	}

	if (1 || this->verbose_p) {
		cout << "VerilogLoader: reading program " << this->load_file << endl;
	}

	assert(verilog_loader::freeloader == 0);
	this->file = new ifstream(this->load_file.c_str(), ios::binary | ios::in);
	if (!this->file->good()) {
		cerr << "VerilogLoader: error opening " << load_file << ": " << std_error_string() << endl;
		return;
	}

	/* This loop is probably slow because it uses getline(). It would probably be faster using a raw buffer. */

	address = -1;
	bytes_num = 0;

	do {
		file->getline(line, BUFFER_SIZE);

		input = line;

		/* Process the line */

		while (*input && (*input != '/')) {

			/* Skip leading spaces */

			while (*input && isspace(*input))
				input++;

			/* Treat start of comment as end of line */

			if (*input == '/')
				break;

			address_flag = 0;

			/* Set address_flag if first character is an at-sign */

			if (*input == '@') {
				address_flag = 1;
				input++;
			}

			/* Parse up to 8 hexadecimal digits */

			for (value=digits=0; digits<8; digits++) {

				c = *input;

				if ((c >= '0') && (c <= '9')) {
					input++;
					value = (value << 4) | (c - '0');
				} else if ((c >= 'a') && (c <= 'f')) {
					input++;
					value = (value << 4) | ((c - 'a') + 10);
				} else if ((c >= 'A') && (c <= 'F')) {
					input++;
					value = (value << 4) | ((c - 'A') + 10);
				} else
					break;
			}

			if (!digits) {
				printf("VerilogLoader.cxx: input file %s not recognized; ignoring\n", this->load_file.c_str());
				return;
			}

			/* Ensure there are no more than 8 hexadecimal digits */

			assert(!isalnum(*input));

			/* Either set the address or store the value */

			if (address_flag) {

				if (bytes_num && (address != -1))
					printf("    address: %08x, bytes: %x\n", address - bytes_num, bytes_num);

				address = value;
				bytes_num = 0;

			} else {

				retval = load_accessor_data->write(address, (sid::big_int_4)value);

				if (retval != bus::ok) {
					printf("VeriogLoader.cxx: failed to write to address: %x\n", (int)value);
					abort();
				}

				/* Ensure there were an even number of digits. */

				assert(!(digits & 1));

				bytes_num += digits / 2;
				address   += digits / 2;
			}
		}

	} while (!file->eof());

	if (bytes_num && (address != -1))
		printf("    address: %08x, bytes: %x\n", address - bytes_num, bytes_num);

	delete this->file;
	this->file = 0;
}

void verilog_loader::check_function_pin_handler (host_int_4 addr)
{
	// Find the function corresponding to the given address in the symbol
	// table, if any, and set current_function to that name. If a function is
	// found, set current_function to a string representing the address.
}

// ----------------------------------------------------------------------------

static vector<string> compLoaderListTypes()
{
	vector<string> types;
	types.push_back("sw-load-Verilog");
	return types;
}

static component* compLoaderCreate(const string& typeName)
{
	if (typeName == "sw-load-Verilog")
		return new verilog_loader();
	else
		return 0;
}

static void compLoaderDelete(component* c)
{
	delete dynamic_cast<verilog_loader*>(c);
}

// static object
DLLEXPORT extern const component_library VerilogLoader_component_library;

const component_library VerilogLoader_component_library =
{
	COMPONENT_LIBRARY_MAGIC,
	&compLoaderListTypes, 
	&compLoaderCreate,
	&compLoaderDelete
};

