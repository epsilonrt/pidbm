/* Copyright © 2020 Pascal JEAN, All rights reserved.
 *
 * Piduino pidbm is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Piduino pidbm is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Piduino pidbm.  If not, see <http://www.gnu.org/licenses/>.
 */
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <pwd.h>
#include <configfile.h>
#include "gpio.h"
#include "connector.h"
#include "pin.h"
#include "soc.h"
#include "pidbm_p.h"
#include "version.h"
#include "config.h"

#ifndef PIDUINO_INSTALL_ETC_DIR
#define PIDUINO_INSTALL_ETC_DIR "/etc/piduino.conf"
#endif

#ifndef PIDUINO_INSTALL_DATA_DIR
#define PIDUINO_INSTALL_DATA_DIR "."
#endif

// Nom du programme en cours défini par la glibc
extern char * program_invocation_name; // glibc
extern char * __progname;

namespace pidbm {

  // ---------------------------------------------------------------------------
  std::string progName() {
#ifdef HAVE_GETPROGNAME_FUNCTION
    return std::string (getprogname ());
#else
    return std::string (__progname);
#endif
  }

  // ---------------------------------------------------------------------------
  bool fileExists (const std::string & path) {
    struct stat fs;

    return (stat (path.c_str(), &fs) == 0);
  }

  // ---------------------------------------------------------------------------
  std::string toUpper (const std::string & s) {
    std::string out (s);

    std::transform (out.begin(), out.end(), out.begin(),
                    std::ptr_fun<int, int> (std::toupper));
    return out;
  }

  // ---------------------------------------------------------------------------
  std::string toLower (const std::string & s) {
    std::string out (s);

    std::transform (out.begin(), out.end(), out.begin(),
                    std::ptr_fun<int, int> (std::tolower));
    return out;
  }
}

using namespace std;
using namespace Popl;
using namespace pidbm;

// ---------------------------------------------------------------------------
//
//                            Pidbm Class
//
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
Pidbm::Pidbm (Pidbm::Private & dd) : d_ptr (&dd) {

}

// ---------------------------------------------------------------------------
Pidbm::Pidbm () :
  d_ptr (new Private (this))  {

}

// ---------------------------------------------------------------------------
Pidbm::~Pidbm() = default;


// ---------------------------------------------------------------------------
bool
Pidbm::open () {

  if (!isOpen()) {
    PIMP_D (Pidbm);

    if (d->findConnectionInfo()) {

      d->db.open (d->cinfo);
      d->checkDatabaseSchemaVersion();
    }
  }
  return isOpen();
}

// ---------------------------------------------------------------------------
void
Pidbm::help (ostream & os) const {
  PIMP_D (const Pidbm);

  os << d->op << endl;
}

// ---------------------------------------------------------------------------
int
Pidbm::parse (int argc, char **argv) {
  PIMP_D (Pidbm);

  d->op.parse (argc, argv);

  if (d->opHelp) {
    cout << d->op << endl;
    return 0;
  }

  if (d->opVersion) {
    version();
    return 0;
  }

  if (d->opWarranty) {
    warranty();
    return 0;
  }

  return argc - 1;
}

// ---------------------------------------------------------------------------
bool
Pidbm::isOpen() const {
  PIMP_D (const Pidbm);

  return d->db.is_open();
}

// ---------------------------------------------------------------------------
void
Pidbm::close() {

  if (isOpen()) {
    PIMP_D (Pidbm);

    d->db.close();
  }
}

// ---------------------------------------------------------------------------
void
Pidbm::exec () {

  if (isOpen()) {
    PIMP_D (Pidbm);

    auto args =  d->op.non_option_args();
    if (args.size() > 0) {

      if (args[0] == "list") {

        d->list();
      }
      else if (args[0] == "add") {

        d->add();
      }
      else if (args[0] == "mod") {

        d->mod();
      }
      else if (args[0] == "rm") {

        d->remove();
      }
      else if (args[0] == "show") {

        d->show();
      }
      else if (args[0] == "cp") {

        d->copy();
      }
      else {

        throw std::invalid_argument ("invalid command: " + args[0]);
      }
    }
    else {

      throw std::invalid_argument ("no command provided");
    }
  }
}

// -----------------------------------------------------------------------------
void
Pidbm::version() {

  cout << "Piduino pidbm version " << VERSION_SHORT << endl;
  cout << "Copyright © 2020 " << Private::Authors << ", " << Private::Website << endl;
  cout << "This program comes with ABSOLUTELY NO WARRANTY." << endl;
  cout << "This is free software, and you are welcome to redistribute it" << endl;
  cout << "under certain conditions; type 'pidbm -w' for details." << endl << endl;
}

// -----------------------------------------------------------------------------
void
Pidbm::warranty () {
  cout <<
       "Copyright © 2020 " << Private::Authors << ", All rights reserved.\n\n"

       " Piduino pidbm is free software: you can redistribute it and/or modify\n"
       " it under the terms of the GNU General Public License as published by\n"
       " the Free Software Foundation, either version 3 of the License, or\n"
       " (at your option) any later version.\n\n"

       " Piduino pidbm is distributed in the hope that it will be useful,\n"
       " but WITHOUT ANY WARRANTY; without even the implied warranty of\n"
       " MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the\n"
       " GNU General Public License for more details.\n\n"

       " You should have received a copy of the GNU General Public License\n"
       " along with Piduino pidbm. If not, see <http://www.gnu.org/licenses/>.\n";
}

// -----------------------------------------------------------------------------
//
//                         Pidbm::Private Class
//
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// static constants
const std::string Pidbm::Private::Authors = "Pascal JEAN";
const std::string Pidbm::Private::Website = "https://github.com/epsilonrt/pidbm";
const std::string Pidbm::Private::Description =
  "usage : pidbm [ options ] {list | show | add | cp | mod | rm | {-v | --version} "
  "{-w | --warranty} | {-h | --help}} [<args>] [ options ]\n"
// 01234567890123456789012345678901234567890123456789012345678901234567890123456789
  "Piduino database manager\n"
  ".... very short description... <TODO>\n"
  "Allowed options";

const std::map<std::string, std::vector<std::string>>
Pidbm::Private::WhatMap = {
  { "arch",         { "arch.id", "arch.name" } },
  {
    "board_family", {
      "board_family.id", "board_family.name", "board_family.i2c_syspath",
      "board_family.spi_syspath", "board_family.uart_syspath"
    }
  },
  { "board_model",  { "id", "name", "board_family_id", "soc_id" } },
  {
    "board",        {
      "board.id", "board.name", "board.board_model_id", "board.gpio_id",
      "board.manufacturer_id", "board.ram", "board.pcb_revision",
      "board.default_i2c_id", "board.default_spi_id", "board.default_uart_id"
    }
  },
  { "gpio",             { "gpio.id", "gpio.name", "gpio.board_family_id" } },
  { "connector_family", { "connector_family.id", "connector_family.name" } },
  {
    "connector", {
      "connector.id", "connector.name", "connector.rows",
      "connector.connector_family_id"
    }
  },
  { "manufacturer", { "manufacturer.id", "manufacturer.name" } },
  { "pin_type",     { "pin_type.id", "pin_type.name" } },
  { "pin_mode",     { "pin_mode.id", "pin_mode.name" } },
  { "pin_name",     { "pin_name.id", "pin_name.name" } },
  {
    "pin_number",   {
      "pin_number.pin_id",
      "pin_number.soc_pin_num", "pin_number.sys_pin_num"
    }
  },
  {
    "pin",   {
      "pin.id",
      "pin_type.id", "pin_type.name",
      "pin_mode.id", "pin_mode.name",
      "pin_name.id", "pin_name.name"
    }
  },
  {
    "pin_gpio",   {
      "gpio_has_pin.gpio_id",
      "pin_type.id", "pin_type.name",
      "pin_mode.id", "pin_mode.name",
      "pin_name.id", "pin_name.name",
      "pin.id",
      "gpio_has_pin.ino_pin_num", "pin_number.soc_pin_num", "pin_number.sys_pin_num"
    }
  },
  {
    "pin_soc",   {
      "pin.id",
      "soc.id", "soc.name",
      "pin_type.id", "pin_type.name",
      "pin_mode.id", "pin_mode.name",
      "pin_name.id", "pin_name.name",
      "pin_number.soc_pin_num", "pin_number.sys_pin_num"
    }
  },
  {
    "soc_family",   {
      "soc_family.id", "soc_family.name",
      "soc_family.arch_id"
    }
  },
  {
    "soc",          {
      "soc.id", "soc.name", "soc.soc_family_id",
      "soc.manufacturer_id", "soc.i2c_count", "soc.spi_count", "soc.uart_count"
    }
  },
};

// -----------------------------------------------------------------------------
// Constructor
Pidbm::Private::Private (Pidbm * q) :
  q_ptr (q), op (Description) {

  op.add<Switch> ("h", "help", "Prints this message", &opHelp);
  op.add<Switch> ("v", "version", "Prints version and exit", &opVersion);
  op.add<Switch> ("w", "warranty", "Output the warranty and exit", &opWarranty);
  op.add<Switch> ("q", "quiet", "Perform operations quietly", &opQuiet);
  opRevision = op.add<Value<std::string>> ("r", "revision", "Board revision");
  opMemory = op.add<Value<std::string>> ("m", "memory", "Board RAM (MB)");
  opTag = op.add<Value<std::string>> ("t", "tag", "Board tag");
  opPCB = op.add<Value<std::string>> ("p", "pcb", "PCB revision");
  opPinMode = op.add<Implicit<std::string>> ("M", "mode", "Pin mode", "input");
  op.add<Value<std::string>> ("c", "connection", "Database connection info", "",
                              &cinfo);
}

// ---------------------------------------------------------------------------
Pidbm::Private::~Private() {

  db.close();
}

// -----------------------------------------------------------------------------
// Use cases

// add manufacturer name
// add manufacturer "Sony Japan"

// add connector
// 0      name
// 1      connector_family  (id/name)
// 2      rows
// add connector "j2-j1" h2x  16
// add connector inner h1x  7

// add gpio name        board_family_id
// add gpio nanopiduo2  1

//  add con2gpio gpio         #connector    connector_id
//  add con2gpio 10           1             17
//  add con2gpio orangepione  1             test

// add pin type input_mode_name [soc_pin_num sys_pin_num]
// add pin video "CVBS"

// add name2pin pin_id mode name [mode name]
// add name2pin 98 alt0 UART1TX alt5 SPI0MISO

// add pin2soc
// 0      soc   (id/name)
// 1      pin_id/pin_name # name is for pin_mode = 'input'
// add pin2soc 3 98
// add pin2soc 3 # <<< interactif

// add pin2con
// 0          connector   (id/name)
// 1          row
// 2          [col]
// 2/3        pin_id/pin_name # name is for pin_mode = 'input'
// add pin2con 17  1 1 2
// add pin2con 17 # <<< interactif

// add pin2gpio
// 0      gpio   (id/name)
// 1      ino_pin_num # must be a unsigned integer
// 2      pin_id/pin_name # name is for pin_mode = 'input'
// add pin2gpio 3 1 GPIOA0
// add pin2gpio 3 1 # <<< interactif, ino_pin_num for the first pin number to request

// add board_model name             board_family  soc
// add board_model "RaspberryPi 4B" 0             5
// add board_model "RaspberryPi 4B" nanopi        H3

// add board  name
// 0          board_model   (id/name)
// 1          gpio          (id/name)
// 2          manufacturer  (id/name)
// 3          default_i2c_id
// 4          default_spi_id
// 5          default_uart_id
//            [-rREVISION] [-mRAM] [-pPCB_REV]
//
// add board "RaspberryPi 4B (0xA03111)" 23 3 1 1 0 0 -r0xa03111 -m1024 -p"1.1"
void Pidbm::Private::add() {

  auto args =  op.non_option_args();

  if (args.size() > 2) {
    cppdb::result records;
    vector<string> what, v;
    string from, to, where, condition;

    vector<string> values (args.cbegin() + 2, args.cend());
    to = args[1];

    // -------------------------------------------------------------------------
    // add manufacturer name
    // add manufacturer "Sony Japan"
    if (to == "manufacturer" && values.size() >= 1) {

      insertRecord ( { "name" }, to, values);
    }

    // -------------------------------------------------------------------------
    // add pin type input_name [soc_pin_num sys_pin_num]
    // add pin video "CVBS"
    else if (to == "pin" && values.size() >= 2) {
      long long t = nameExists ("pin_type", values[0], true);

      if (t >= 0) {
        long long n;
        string pin_name_id, pin_id, pin_type_id;

        if (t == Pin::Type::Gpio && values.size() < 4) {

          throw std::invalid_argument ("You must provide 2 numbers for a GPIO pin (soc, system).");
        }
        pin_type_id = to_string (t);

        n = nameExists ("pin_name", values[1]);
        if (n >= 0) {

          pin_name_id = to_string (n);
          if (! opQuiet) {
            cout <<  "pin_name '" << values[1] << "' found (id:" << pin_name_id << "), nothing to add." << endl;
          }
        }
        else {

          v.push_back (values[1]);
          n = insertRecord ( { "name" }, "pin_name", v);
          pin_name_id = to_string (n);
        }

        what = {"pin.id"};
        from.assign ("pin "
                     "INNER JOIN pin_has_name ON pin.id=pin_has_name.pin_id "
                    );
        where = "pin_mode_id=? AND pin_name_id=? AND pin_type_id=?";
        v.clear();
        v.push_back ("0");
        v.push_back (pin_name_id);
        v.push_back (pin_type_id);
        n = selectRecord (records, what, from, where, v);
        if (records.next()) {

          records >> pin_id;
          if (! opQuiet) {
            cout << "pin with name '" << values[1] << "' and type "
                 << values[0] << " found (id:"  << pin_id << "), nothing to add."
                 << endl;
          }
        }
        else {

          v.clear();
          v.push_back (pin_type_id);
          n = insertRecord ( { "pin_type_id" }, "pin", v);
          pin_id = to_string (n);

          v.clear();
          v.push_back (pin_id);
          v.push_back (pin_name_id);
          v.push_back ("0");
          insertRecord ( { "pin_id", "pin_name_id", "pin_mode_id" }, "pin_has_name", v);
        }

        if (t == Pin::Type::Gpio) {

          v.assign (values.begin() + 2, values.end());

          from = "pin_number";
          what = WhatMap.at (from);
          where = "pin_id";
          n = selectRecordEqual (records, what, from, where, pin_id);

          if (records.next()) {

            records >> pin_id;
            what.erase (what.begin());
            if (! opQuiet) {

              cout << from << " record found (id:" << pin_id <<
                   "), has been updated." << endl;
            }
            updateRecord (what, from, "pin_id=?", {pin_id}, v);
          }
          else {

            v.insert (v.begin(), pin_id);
            insertRecord (what, from, v);
          }
        }
      }
      else {

        throw std::invalid_argument (values[0] +
                                     " invalid pin type name, use "
                                     "`list pin_type` to see them.");
      }
    }

    // -------------------------------------------------------------------------
    // add name2pin pin_id mode name [mode name]
    // add name2pin 98 alt0 UART1TX alt5 SPI0MISO
    else if (to == "name2pin" && values.size() >= 3) {
      string pin_id;

      if (searchPinId (values[0], pin_id)) {

        selectRecordEqual (records, {"pin_type_id"}, "pin", "id", pin_id);
        if (records.next()) {
          long long pin_type_id;

          records >> pin_type_id;
          if (pin_type_id == Pin::Type::Gpio) {

            for (size_t n = 1; n < (values.size() - 1); n += 2) {
              long long pin_mode_id;

              pin_mode_id = nameExists ("pin_mode", values[n], true);
              if (pin_mode_id >= 0) {
                long long pin_name_id;

                v.clear();
                pin_name_id = nameExists ("pin_name", values[n + 1]);
                if (pin_name_id < 0) {

                  v.push_back (values[n + 1]);
                  pin_name_id = insertRecord ( {"name"}, "pin_name", v);
                  v.clear();
                }
                v.push_back (pin_id);
                v.push_back (to_string (pin_name_id));
                v.push_back (to_string (pin_mode_id));

                insertRecord ( { "pin_id", "pin_name_id", "pin_mode_id" },
                               "pin_has_name", v, true);
              }
              else {

                throw std::invalid_argument (values[n + 1] + " invalid pin mode "
                                             "name, use `list pin_mode` to see them.");
              }
            }
          }
          else {

            throw std::invalid_argument (values[0] + " is not a GPIO type pin.");
          }
        }

      }
      else {

        throw std::invalid_argument ("pin " + values[0] + " not found !");
      }
    }

    // -------------------------------------------------------------------------
    // add pin2gpio
    // 0      gpio   (id/name)
    // 1      ino_pin_num # must be a unsigned integer
    // 2      pin_id/pin_name # name is for pin_mode = 'input'
    // add pin2gpio 3 1 GPIOA0
    // add pin2gpio 3 1 # <<< interactif, ino_pin_num for the first pin number to request
    else if (to == "pin2gpio" && values.size() >= 2) {
      string gpio_id;

      if (readArg (0, "gpio", gpio_id, true)) {
        vector<string> pv, pn;
        int num;
        std::size_t p = 0;

        num = stoi (values[1], &p); // thrown exception if not int
        if (p != values[1].size()) {

          throw std::invalid_argument (values[1] + " invalid pin number.");
        }

        if (values.size() < 3) {
          string str;
          bool quit;
          string  gpio_name;
          int n = num;

          selectRecordEqual (records, {"name"}, "gpio", "id", gpio_id);
          if (records.next()) {

            records >> gpio_name;
          }

          cout << "-- Adds pins to the " << gpio_name << " Gpio (id:" << gpio_id
               << ") --" << endl
               << "Enter the pin one by one then ENTER, press [q/Q] to exit." << endl;

          do {

            cout << "Pin #" << n << ", pin_id or pin_name ? ";
            cin >> str;
            quit = (str == "Q" || str == "q");

            if (!quit) {
              string pin_id;

              if (searchPinId (str, pin_id)) {

                selectRecordEqual (records, {"pin_type_id"}, "pin", "id", pin_id);
                if (records.next()) {
                  long long pin_type_id;

                  records >> pin_type_id;
                  if (pin_type_id != Pin::Type::Gpio) {

                    throw std::invalid_argument (str + " is not a GPIO type pin.");
                  }
                }

                pv.push_back (pin_id);
                pn.push_back (to_string (n++));
              }
              else {

                cout << "pin " << str << " not found !" << endl;
              }
            }
          }
          while (!quit);
        }
        else {
          string pin_id;

          if (searchPinId (values[2], pin_id)) {

            pv.push_back (pin_id);
            pn.push_back (to_string (num));
          }
          else {

            cout << "pin " << values[2] << " not found !" << endl;
          }
        }

        for (size_t i = 0; i < pv.size(); i++) {
          string pin_id = pv[i];
          string ino_pin_num = pn[i];

          v = {  gpio_id, pin_id, ino_pin_num};
          insertRecord ( { "gpio_id", "pin_id", "ino_pin_num"}, "gpio_has_pin", v, true);
        }
      }
      else {

        throw std::invalid_argument ("invalid gpio " + values[0] + ", use `list gpio` to see them.");
      }
    }

    // -------------------------------------------------------------------------
    // add pin2soc
    // 0      soc   (id/name)
    // 1      pin_id/pin_name # name is for pin_mode = 'input'
    // add pin2soc 3 98
    // add pin2soc 3 # <<< interactif
    else if (to == "pin2soc" && values.size() >= 1) {
      string soc_id;

      if (readArg (0, "soc", soc_id, true)) {
        string  soc_name;
        vector<string> pv;

        selectRecordEqual (records, {"name"}, "soc", "id", soc_id);
        if (records.next()) {

          records >> soc_name;
        }

        if (values.size() < 2) {
          string str;
          bool quit;

          cout << "-- Adds pins to the " << soc_name << " SoC (id:" << soc_id
               << ") --" << endl
               << "Enter the pin one by one then ENTER, press [q/Q] to exit." << endl;

          do {

            cout << "? ";
            cin >> str;
            quit = (str == "Q" || str == "q");

            if (!quit) {
              string pin_id;

              if (searchPinId (str, pin_id)) {

                pv.push_back (pin_id);
              }
              else {

                cout << "pin " << str << " not found !" << endl;
              }
            }
          }
          while (!quit);
        }
        else {
          string pin_id;

          if (searchPinId (values[1], pin_id)) {

            pv.push_back (pin_id);
          }
          else {

            cout << "pin " << values[1] << " not found !" << endl;
          }
        }

        for (auto pin_id : pv) {

          v = { pin_id, soc_id };
          insertRecord ( {"pin_id", "soc_id"}, "soc_has_pin", v, true);
        }
      }
      else {

        throw std::invalid_argument ("invalid soc " + values[0] + ", use `list soc` to see them.");
      }
    }

    // -------------------------------------------------------------------------
    // add pin2con
    // 0          connector   (id/name)
    // 1          row
    // 2          [col]
    // 2/3        pin_id/pin_name # name is for pin_mode = 'input'
    // add pin2con 17  1 1 2
    // add pin2con 17 # <<< interactif
    else if (to == "pin2con" && values.size() >= 1) {
      long long connector_id;

      if (readArg (0, "connector", connector_id, true)) {
        Connector c (db, connector_id);
        size_t row, col;
        long long pin_id;

        if (values.size() >= 3) {

          row = stoul (values[1], nullptr);
          if (values.size() >= 4 && c.columns() > 1) {

            col = stoul (values[2], nullptr);
            pin_id = stol (values[3], nullptr);
          }
          else {

            col = 1;
            pin_id = stol (values[2], nullptr);
          }

          c.updatePin (row, col, pin_id);
        }
        else {
          string line;
          bool quit;
          size_t colwidth = string (to_string (c.pinNumber (c.rows(), c.columns()))).size();

          cout << "-- Adds pins to the " << c.name() << " " << c.rows() << "x"
               << c.columns() << " connector (id:" << connector_id << ") --" << endl
               << "Enter the pin identifiers of the pin numbers (opposite) separated by space or comma then ENTER, press [q/Q] to exit." << endl;

          for (size_t row = 1; row <= c.rows() && ! quit;) {

            for (size_t col = 1; col <= c.columns(); col++) {

              cout << "| " << c.formatColumn (to_string (c.pinNumber (row, col)),
                                              colwidth, Connector::Alignment::Left) << " ";
            }
            cout << "| ? ";

            getline (cin, line);
            quit = (line == "Q" || line == "q");

            if (!quit) {
              size_t comma, pos = 0;
              vector<long long> pv;

              for (size_t col = 1; col <= c.columns(); col++) {
                string word;
                long long pin_id;

                comma = line.find (',', pos);

                word = line.substr (pos, comma);
                if (searchPinId (word, pin_id)) {

                  pv.push_back (pin_id);
                }
                else {

                  cout << "pin " << word << " not found !" << endl;
                  break;
                }

                pos += comma + 1;
              }

              if (pv.size() == c.columns()) {

                for (size_t col = 1; col <= c.columns() ; col++) {

                  c.updatePin (row, col, pv[col - 1]);
                }
                row++;
              }
            }
          }
          cout << c;
        }
      }
      else {

        throw std::invalid_argument ("invalid connector " + values[0] + ", use `list connector` to see them.");
      }
    }

    // -------------------------------------------------------------------------
    // add connector
    // 0      name
    // 1      connector_family  (id/name)
    // 2      rows
    // add connector "j2-j1" h2x  16
    // add connector inner h1x  7
    else if (to == "connector" && values.size() >= 3) {
      string connector_family_id;

      if (readArg (1, "connector_family", connector_family_id, true)) {

        v.push_back (connector_family_id);
        v.push_back (values[0]);
        v.push_back (values[2]);
        insertRecord ( {"connector_family_id", "name", "rows"}, to, v);
      }
      else {

        throw std::invalid_argument ("invalid connector_family " + values[1] +
                                     ", use `list connector_family` to see them.");
      }
    }

    // -------------------------------------------------------------------------
    // add gpio name        board_family
    // add gpio nanopiduo2  1
    else if (to == "gpio" && values.size() >= 2) {
      string board_family_id;

      if (readArg (1, "board_family", board_family_id, true)) {

        v.push_back (values[0]);
        v.push_back (board_family_id);
        insertRecord ( {"name", "board_family_id"}, to, v, true);
      }
      else {

        throw std::invalid_argument ("invalid board_family " + values[1] +
                                     ", use `list board_family` to see them.");
      }
    }

    // -------------------------------------------------------------------------
    //  add con2gpio gpio         #connector    connector_id
    //  add con2gpio 10           1             17
    //  add con2gpio orangepione  1             test
    else if (to == "con2gpio" && values.size() >= 3) {
      string gpio_id;

      if (readArg (0, "gpio", gpio_id, true)) {
        string connector_id;

        if (readArg (2, "connector", connector_id, true)) {

          v.push_back (gpio_id);
          v.push_back (values[1]);
          v.push_back (connector_id);
          insertRecord ( {"gpio_id", "num", "connector_id"},
                         "gpio_has_connector", v, true);
        }
        else {

          throw std::invalid_argument ("invalid connector " + values[2] +
                                       ", use `list connector` to see them.");
        }
      }
      else {

        throw std::invalid_argument ("invalid gpio " + values[0] +
                                     ", use `list gpio` to see them.");
      }
    }

    // -------------------------------------------------------------------------
    // add board_model name             board_family  soc
    // add board_model "RaspberryPi 4B" 0             5
    // add board_model "RaspberryPi 4B" nanopi        H3
    else if (to == "board_model" && values.size() >= 3) {
      string board_family_id;

      if (readArg (1, "board_family", board_family_id, true)) {
        string soc_id;

        if (readArg (2, "soc", soc_id, true)) {

          v.push_back (values[0]);
          v.push_back (board_family_id);
          v.push_back (soc_id);
          insertRecord ( {"name", "board_family_id", "soc_id"}, to, v, true);
        }
        else {

          throw std::invalid_argument ("invalid SoC " + values[2] +
                                       ", use `list soc` to see them.");
        }
      }
      else {

        throw std::invalid_argument ("invalid board_family " + values[1] +
                                     ", use `list board_family` to see them.");
      }
    }

    // -------------------------------------------------------------------------
    // add board
    // 0          name
    // 1          board_model   (id/name)
    // 2          gpio          (id/name)
    // 3          manufacturer  (id/name)
    // 4          default_i2c_id
    // 5          default_spi_id
    // 6          default_uart_id
    //            [-rREVISION] [-mRAM] [-pPCB_REV]
    //
    // add board "RaspberryPi 4B (0xA03111)" 23 3 1 1 0 0 -r0xa03111 -m1024 -p"1.1"
    else if (to == "board" && values.size() >= 7) {

      if ( (opRevision->is_set() || opTag->is_set()) &&
           (opRevision->is_set() != opTag->is_set())) {
        string board_model_id;


        if (readArg (1, "board_model", board_model_id, true)) {
          string gpio_id;

          values[1] = board_model_id;
          if (readArg (2, "gpio", gpio_id, true)) {
            string manufacturer_id;

            values[2] = gpio_id;
            if (readArg (3, "manufacturer", manufacturer_id, true)) {
              long long id;
              unsigned long rev;
              what = { "name",
                       "board_model_id",
                       "gpio_id",
                       "manufacturer_id",
                       "default_i2c_id",
                       "default_spi_id",
                       "default_uart_id"
                     };

              values[3] = manufacturer_id;
              
              if (opMemory->is_set()) {

                what.push_back ("ram");
                values.push_back (opMemory->value());
              }

              if (opPCB->is_set()) {

                what.push_back ("pcb_revision");
                values.push_back (opPCB->value());
              }

              if (opRevision->is_set()) {

                rev = std::stol (opRevision->value(), nullptr, 0); // check if rev valid
              }

              id = insertRecord (what, to, values);

              if (id >= 0) {

                what = { "board_id" };
                values.clear();
                values.push_back (to_string (id));

                if (opRevision->is_set()) {

                  to = "revision";
                  values.push_back (to_string (rev));
                }
                else if (opTag->is_set()) {

                  to = "tag";
                  values.push_back (opTag->value());
                }

                what.push_back (to);
                insertRecord (what, to, values);
              }
            }
            else {

              throw std::invalid_argument ("invalid manufacturer " + values[2] +
                                           ", use `list manufacturer` to see them.");
            }
          }
          else {

            throw std::invalid_argument ("invalid gpio " + values[1] +
                                         ", use `list gpio` to see them.");
          }
        }
        else {

          throw std::invalid_argument ("invalid board_model " + values[0] +
                                       ", use `list board_model` to see them.");
        }
      }
      else {

        throw std::invalid_argument ("You must provide a revision with -r OR tag with -t to add a board.");
      }
    }
    else {

      throw std::invalid_argument ("invalid sub-command: " + to);
    }
  }
  else {

    throw std::invalid_argument ("nothing to add !");
  }
}

// -----------------------------------------------------------------------------
// Use cases

// rm board [name_like/id]
// rm board_model [name_like/id]
// rm gpio [name_like/id]
// rm connector [name_like/id]
// rm manufacturer [name_like/id]
// rm pin_name [name_like/id]
void Pidbm::Private::remove() {

  auto args =  op.non_option_args();

  if (args.size() > 1) {

    if (args.size() > 2) {
      string where;
      string condition;
      string from (args[1]);
      bool like = false;

      if (from == "board") {
        long long n;
        cppdb::result records;
        vector<string> what;
        vector<string> idList;

        setWhereCondition (2, where, condition, like);

        what = { "id" };
        n = selectRecordEqual (records, what, from, where, condition);

        if (n > 0) {
          string response;


          if (opQuiet) {

            response = "Y";
          }
          else {

            printRecordEqual (WhatMap.at ("board"), from, where, condition, like);
            cout << "Could you confirm the deletion of the " << n << "records above [y/N] ?  ";
            cin >> response;
          }

          if (response == "y" || response == "Y") {
            string id;
            bool quiet = opQuiet;

            opQuiet = true;
            while (records.next()) {

              records >> id;
              deleteRecord (what, "revision", "board_id", id);
              deleteRecord (what, "tag", "board_id", id);
            }

            deleteRecord (what, from, where, condition, like);
            opQuiet  = quiet;
          }
        }
      }
      else if (from == "board_model" && from == "gpio" && from == "connector" &&
               from == "manufacturer" && from == "pin_name") {

        setWhereCondition (2, where, condition, like);
        deleteRecord (WhatMap.at (from), from, where, condition, like);
      }
      else {

        throw std::invalid_argument ("invalid table " + from);
      }
    }
    else {

      throw std::invalid_argument ("Could you tell me what to erase ?");
    }
  }
  else {

    throw std::invalid_argument ("no command provided");
  }
}

// -----------------------------------------------------------------------------
// Use cases

// cp connector [name_like/id] new_name
// cp soc [name_like/id] new_name
void Pidbm::Private::copy() {

  auto args =  op.non_option_args();

  if (args.size() > 1) {
    string to (args[1]);

    // cp connector [name_like/id] new_name
    if (to == "connector" && args.size() >= 4) {
      long long connector_id;

      if (readArg (0, "connector", connector_id, true)) {

        Connector src (db, connector_id);
        Connector dst (src, args[3]);
        if (!opQuiet) {

          cout << src.name() << " connector (id:" << src.id() << ") copied to "
               << dst.name() << " connector (id:" << dst.id() << ")." << endl;
        }
      }
    }

    // cp soc [name_like/id] new_name
    else if (to == "soc" && args.size() >= 4) {
      long long soc_id;

      if (readArg (0, "soc", soc_id, true)) {

        Soc src (db, soc_id);
        Soc dst (src, args[3]);
        if (!opQuiet) {

          cout << src.name() << " soc (id:" << src.id() << ") copied to "
               << dst.name() << " soc (id:" << dst.id() << ")." << endl;
        }
      }
    }
    else {

      throw std::invalid_argument ("invalid item " + to);
    }
  }
  else {

    throw std::invalid_argument ("no argument provided");
  }
}

// -----------------------------------------------------------------------------
// Use cases

// show connector [name_like/id]
// show gpio [name_like/id]
void Pidbm::Private::show() {
  string from;
  string where;
  string condition;
  vector<string> what;
  bool like = false;

  auto args =  op.non_option_args();

  if (args.size() > 1) {

    from = args[1];
    if (from == "connector" && args.size() > 2) {
      cppdb::result records;

      what =  { "id" };
      setWhereCondition (2, where, condition, like);

      selectRecordEqual (records, what, from, where, condition, like);
      while (records.next()) {
        long long id;
        records >> id;
        Connector c (db, id);

        cout << c;
      }
    }
    else if (from == "gpio" && args.size() > 2) {
      cppdb::result records;

      what =  { "id" };
      setWhereCondition (2, where, condition, like);

      selectRecordEqual (records, what, from, where, condition, like);
      while (records.next()) {
        long long id;
        records >> id;
        Gpio c (db, id);

        cout << c;
      }
    }
    else {

      throw std::invalid_argument ("invalid table " + from);
    }
  }
  else {

    throw std::invalid_argument ("no argument provided");
  }
}

// -----------------------------------------------------------------------------
// Use cases

// mod board_family id/name new_name new_i2c_syspath new_spi_syspath new_uart_syspath
// mod board_model id/name new_name new_board_family_id new_soc_id
// mod board id/name new_name new_board_model_id new_gpio_id new_manufacturer_id new_ram new_pcb_revision new_default_i2c_id new_default_spi_id new_default_uart_id
// mod gpio id/name new_name new_board_family_id
// mod connector id/name new_name new_rows new_connector_family_id
// mod manufacturer id/name new_name
// mod pin_type id/name new_name
// mod pin_mode id/name new_name
// mod pin_name id/name new_name
void Pidbm::Private::mod() {
  auto args =  op.non_option_args();

  if (args.size() > 1) {
    string to (args[1]);

    if (WhatMap.count (to) > 0 && to == "board_family" && to == "board_model" &&
        to == "board" && to == "gpio" && to == "connector" &&
        to == "manufacturer" && to == "pin_type" && to == "pin_mode" &&
        to == "pin_name") {

      if (args.size() > 2) {
        string where;
        string c;
        bool like;

        setWhereCondition (2, where, c, like);

        if (where.size() && c.size()) {
          vector<string> what;
          bool wfound = false;

          what = WhatMap.at (to);

          for (auto w : what) {
            size_t pos = w.rfind (where);
            if (pos > 0) {
              if (w[pos - 1] == '.') {
                wfound = true;
              }
            }
            else {
              wfound = true;
            }
          }

          if (wfound) {
            vector<string> values (args.cbegin() + 2, args.cend());
            vector<string> condition;

            condition.push_back (c);

            where += "=?";
            updateRecord (what, to, where, condition, values);
          }
        }

      }
      else {

        throw std::invalid_argument ("nothing data provided to modify " + to);
      }
    }
    else {

      throw std::invalid_argument ("invalid table " + to);
    }

  }
  else {

    throw std::invalid_argument ("no argument provided");
  }
}

// -----------------------------------------------------------------------------
// Use cases

// list board [board_name_like/board_id]
// list board revision [revision_value]
// list board tag [tag_value]

// list gpio [name_like/id]
// list gpio pin [gpio_name_like/gpio_id] [-Mpin_mode]

// list connector [name_like/id]
// list connector gpio gpio_name_like/gpio_id
// list connector board board_name_like/board_id

// list pin [pin_name_like/pin_id] [-Mpin_mode]

// list pin gpio [pin_name_like/pin_id] [-Mpin_mode]
// list pin net [pin_name_like/pin_id] [-Mpin_mode]
// list pin audio [pin_name_like/pin_id] [-Mpin_mode]
// list pin power [pin_name_like/pin_id] [-Mpin_mode]
// list pin usb [pin_name_like/pin_id] [-Mpin_mode]
// list pin video [pin_name_like/pin_id] [-Mpin_mode]
// list pin nc [pin_name_like/pin_id] [-Mpin_mode]

// list pin gpio soc [soc_name_like/soc_id] [-Mpin_mode]
// list pin net soc [soc_name_like/soc_id] [-Mpin_mode]
// list pin audio soc [soc_name_like/soc_id] [-Mpin_mode]
// list pin power soc [soc_name_like/soc_id] [-Mpin_mode]
// list pin usb soc [soc_name_like/soc_id] [-Mpin_mode]
// list pin video soc [soc_name_like/soc_id] [-Mpin_mode]
// list pin nc soc [soc_name_like/soc_id] [-Mpin_mode]

// list pin soc 3 [-Mpin_mode]
// list pin soc H5 [-Mpin_mode]

// list arch [name_like/id]
// list soc [name_like/id]
// list soc_family [name_like/id]
// list manufacturer [name_like/id]
// list board_family [name_like/id]
// list board_model [name_like/id]
// list connector_family [name_like/id]
// list pin_type [name_like/id]
// list pin_mode [name_like/id]
// list pin_name [name_like/id]
// list pin_number
void Pidbm::Private::list() {

  auto args =  op.non_option_args();

  if (args.size() > 1) {
    string where;
    string condition;
    string orderby;
    string from (args[1]);
    vector<string> what;
    bool like = false;
    int pos = 2;

    if (WhatMap.count (from) > 0) {
      what = WhatMap.at (from);
    }

    // list board [board_name_like/board_id]
    // list board revision [revision_value]
    // list board tag [tag_value]
    if (from == "board") {

      if (args.size() > 2) {
        vector<string>::iterator it = what.begin();

        if (args[2] == "revision") {

          what.insert (it, "%revision");
          from.assign ("revision INNER JOIN board ON revision.board_id=board.id");
          where = "revision";
          pos = 3;
        }
        else if (args[2] == "tag") {

          what.insert (it, "tag");
          from.assign ("tag INNER JOIN board ON tag.board_id=board.id");
          where = "tag";
          pos = 3;
        }
      }

      setWhereCondition (pos, where, condition, like);
    }

    // list connector [name_like/id]
    // list connector gpio gpio_name_like/gpio_id
    // list connector board board_name_like/board_id
    else if (from == "connector") {
      string orderby;

      if (args.size() > 3) {
        string sub_command (args[2]);
        vector<string>::iterator it = what.begin();

        pos = 3;
        orderby = "gpio_has_connector.num";
        it = what.insert (it, "num");

        if (sub_command == "gpio") {

          where = "gpio.";
          from.assign ("connector INNER JOIN gpio_has_connector ON connector_id=connector.id "
                       "INNER JOIN gpio ON gpio_has_connector.gpio_id=gpio.id");
        }
        else if (sub_command == "board") {

          where = "board.";
          from.assign ("connector INNER JOIN gpio_has_connector ON connector_id=connector.id "
                       "INNER JOIN board ON board.gpio_id=gpio_has_connector.gpio_id");
        }
        else {

          throw std::invalid_argument ("invalid sub-command " + sub_command);
        }
      }

      setWhereCondition (pos, where, condition, like);
    }

    // list pin [pin_name_like/pin_id] [-Mpin_mode]

    // list pin gpio [pin_name_like/pin_id] [-Mpin_mode]
    // list pin net [pin_name_like/pin_id] [-Mpin_mode]
    // list pin audio [pin_name_like/pin_id] [-Mpin_mode]
    // list pin power [pin_name_like/pin_id] [-Mpin_mode]
    // list pin usb [pin_name_like/pin_id] [-Mpin_mode]
    // list pin video [pin_name_like/pin_id] [-Mpin_mode]
    // list pin nc [pin_name_like/pin_id] [-Mpin_mode]

    // list pin gpio soc [soc_name_like/soc_id] [-Mpin_mode]
    // list pin net soc [soc_name_like/soc_id] [-Mpin_mode]
    // list pin audio soc [soc_name_like/soc_id] [-Mpin_mode]
    // list pin power soc [soc_name_like/soc_id] [-Mpin_mode]
    // list pin usb soc [soc_name_like/soc_id] [-Mpin_mode]
    // list pin video soc [soc_name_like/soc_id] [-Mpin_mode]
    // list pin nc soc [soc_name_like/soc_id] [-Mpin_mode]

    // list pin soc 3 [-Mpin_mode]
    // list pin soc H5 [-Mpin_mode]
    else if (from == "pin") {
      vector<string> cv;

      from.assign ("pin "
                   "INNER JOIN pin_type on pin_type.id = pin.pin_type_id "
                   "INNER JOIN pin_has_name on pin.id = pin_has_name.pin_id "
                   "INNER JOIN pin_name on pin_name.id = pin_has_name.pin_name_id "
                   "INNER JOIN pin_mode on pin_mode.id = pin_has_name.pin_mode_id "
                  );

      what = WhatMap.at ("pin");

      if (args.size() > 2) { // pin +
        string & sub_command = args[2];

        long long t = nameExists ("pin_type", sub_command, true);

        if (t >= 0) { // pin + gpio,power,usb,audio,video,nc,net

          if (t == Pin::Type::Gpio) {
            from.assign ("pin "
                         "INNER JOIN pin_type on pin_type.id = pin.pin_type_id "
                         "INNER JOIN pin_has_name on pin.id = pin_has_name.pin_id "
                         "INNER JOIN pin_name on pin_name.id = pin_has_name.pin_name_id "
                         "INNER JOIN pin_mode on pin_mode.id = pin_has_name.pin_mode_id "
                         "INNER JOIN pin_number on pin.id = pin_number.pin_id "
                         "INNER JOIN gpio_has_pin on pin.id = gpio_has_pin.pin_id "
                        );
            what = WhatMap.at ("pin_gpio");
          }

          if (args.size() > 4) {

            if (args[3] == "soc") {

              where = "soc.";
              setWhereCondition (4, where, condition, like);
              if (where.size() && condition.size()) {

                from += "INNER JOIN soc_has_pin on pin.id = soc_has_pin.pin_id "
                        "INNER JOIN soc on soc.id = soc_has_pin.soc_id ";
                where += (like ? " LIKE ? AND " : "=? AND ");
                cv.push_back (condition);
              }

            }
          }

          if (where.empty()) {

            setWhereCondition (3, where, condition, like);
            if (where.size() && condition.size()) {
              if (where == "name") {
                where = "pin_name.name";
              }
              else if (where == "id") {
                where = "pin.id";
              }
              where += (like ? " LIKE ? AND " : "=? AND ");
              cv.push_back (condition);
            }
          }

          where += "pin.pin_type_id=?";
          cv.push_back (to_string (t));
          orderby = "pin_name.name";
        } // <<< pin + gpio,power,usb,audio,video,nc,net
        else if (sub_command == "soc" && args.size() > 3) { // pin + soc >>>
          from.assign ("pin "
                       "INNER JOIN pin_type on pin_type.id = pin.pin_type_id "
                       "INNER JOIN pin_has_name on pin.id = pin_has_name.pin_id "
                       "INNER JOIN pin_name on pin_name.id = pin_has_name.pin_name_id "
                       "INNER JOIN pin_mode on pin_mode.id = pin_has_name.pin_mode_id "
                       "INNER JOIN soc_has_pin on pin.id = soc_has_pin.pin_id "
                       "INNER JOIN soc on soc.id = soc_has_pin.soc_id "
                      );

          where = "soc.";
          orderby = "pin_name.name";
          setWhereCondition (3, where, condition, like);
          if (where.size() && condition.size()) {

            where += (like ? " LIKE ?" : "=?");
            cv.push_back (condition);
          }
        }  // <<< pin + soc
      }  // <<< pin +

      if (where.empty()) { // pin only

        setWhereCondition (pos, where, condition, like);
        orderby = "pin_name.name";
        if (where.size() && condition.size()) {

          cv.push_back (condition);
          if (where == "name") {

            where = "pin_name.name LIKE ?";
          }
          else if (where == "id") {

            where = "pin.id=?";
          }
        }
      }

      if (opPinMode->is_set()) {
        string pin_mode_id;

        readArg (opPinMode->value(), "pin_mode", pin_mode_id, true);
        if (pin_mode_id.size()) {

          if (where.size()) {

            where += " AND";
          }
          where += " pin_mode.id=?";
          cv.push_back (pin_mode_id);
        }
      }

      printRecord (what, from, where, cv, orderby);
      return; // << --------------------------- return
    } //-- < pin

    // list gpio [name_like/id]
    // list gpio pin [gpio_name_like/gpio_id]  [-Mpin_mode]
    else if (from == "gpio") {

      if (args.size() > 2) { // gpio +

        if (args[2] == "pin") {
          vector<string> cv;

          from.assign ("gpio_has_pin "
                       "INNER JOIN gpio on gpio.id = gpio_has_pin.gpio_id "
                       "INNER JOIN pin on pin.id = gpio_has_pin.pin_id "
                       "INNER JOIN pin_type on pin_type.id = pin.pin_type_id "
                       "INNER JOIN pin_has_name on pin.id = pin_has_name.pin_id "
                       "INNER JOIN pin_name on pin_name.id = pin_has_name.pin_name_id "
                       "INNER JOIN pin_mode on pin_mode.id = pin_has_name.pin_mode_id "
                       "INNER JOIN pin_number on pin.id = pin_number.pin_id "
                      );

          what = WhatMap.at ("pin_gpio");

          setWhereCondition (3, where, condition, like);
          if (where.size() && condition.size()) {
            if (where == "name") {
              where = "gpio.name";
            }
            else if (where == "id") {
              where = "gpio_has_pin.gpio_id";
            }
            where += (like ? " LIKE ?" : "=?");
            cv.push_back (condition);
          }

          orderby = "ino_pin_num";
          if (opPinMode->is_set()) {
            string pin_mode_id;

            readArg (opPinMode->value(), "pin_mode", pin_mode_id, true);
            if (pin_mode_id.size()) {

              if (where.size()) {

                where += " AND";
              }
              where += " pin_mode.id=?";
              cv.push_back (pin_mode_id);
            }
          }

          printRecord (what, from, where, cv, orderby);
          return; // << --------------------------- return
        }

      }

      setWhereCondition (pos, where, condition, like);
      what = WhatMap.at (from);
    }
    // list arch [name_like/id]
    // list soc [name_like/id]
    // list soc_family [name_like/id]
    // list manufacturer [name_like/id]
    // list board_family [name_like/id]
    // list board_model [name_like/id]
    // list connector_family [name_like/id]
    // list pin_type [name_like/id]
    // list pin_mode [name_like/id]
    // list pin_name [name_like/id]
    // list pin_number
    else if (WhatMap.count (from))  {

      setWhereCondition (pos, where, condition, like);
      what = WhatMap.at (from);
    }
    else {

      throw std::invalid_argument ("invalid table " + from);
    }

    printRecordEqual (what, from, where, condition, like, orderby);
  }
  else {

    throw std::invalid_argument ("no table provided");
  }
}

// -----------------------------------------------------------------------------
long long Pidbm::Private::selectRecordEqual (cppdb::result & res,
    const std::vector<std::string> & what,
    const std::string & from,
    const std::string & where,
    const std::string & condition,
    bool like,
    const std::string & orderby,
    const std::string & groupby) {
  std::ostringstream req;
  std::vector<std::string> v;

  if (where.size() && condition.size()) {
    req << where << (like ? " LIKE " : "=") << "?";
    v.push_back (condition);
  }
  return selectRecord (res, what, from, req.str(), v, orderby, groupby);
}

// -----------------------------------------------------------------------------
void Pidbm::Private::deleteRecord (const std::vector<std::string> & what,
                                   const std::string & from,
                                   const std::string & where,
                                   const std::string & condition,
                                   bool like) {
  string response;

  if (opQuiet) {

    response = "Y";
  }
  else {

    long long n = printRecordEqual (what, from, where, condition, like);
    cout << "Could you confirm the deletion of the " << n << "records above [y/N] ?  ";
    cin >> response;
  }

  if (response == "y" || response == "Y") {
    cppdb::statement stat;
    std::ostringstream req;

    req << "DELETE FROM " << from << " WHERE " << where << (like ? " LIKE " : "=") << "?";

    // cout << req.str() << endl; // debug
    stat = db << req.str() << condition;
    stat.exec();
  }
}

// -----------------------------------------------------------------------------
void Pidbm::Private::updateRecord (const std::vector<std::string> & what,
                                   const std::string & to,
                                   const std::string & where,
                                   const std::vector<std::string> & condition,
                                   const std::vector<std::string> & values) {

  std::ostringstream req;
  cppdb::statement st;

  req << "UPDATE " << to << " SET ";

  for (size_t i = 0; i < what.size(); i++) {

    size_t pos = what[i].rfind ('.');
    if (pos == string::npos) {

      req << what[i];
    }
    else {
      req << what[i].substr (pos + 1);
    }
    req << "=?";
    if (i < (what.size() - 1)) {
      req << ',';
    }
  }
  req << " WHERE " << where;
  //cout << req.str() << endl; // debug

  st = db << req.str();
  for (auto v : values) {
    st << v;
  }
  for (auto c : condition) {
    st << c;
  }

  st.exec();
  if (!opQuiet) {

    cout << st.affected() << " record updated to " << to << "." << endl;
  }
}

// -----------------------------------------------------------------------------
long long Pidbm::Private::printRecordEqual (const std::vector<std::string> & what,
    const std::string & from,
    const std::string & where,
    const std::string & condition,
    bool like,
    const std::string & orderby,
    const std::string & groupby) {
  std::ostringstream req;
  std::vector<std::string> v;

  if (where.size() && condition.size()) {
    req << where << (like ? " LIKE " : "=") << "?";
    v.push_back (condition);
  }
  return printRecord (what, from, req.str(), v, orderby, groupby);
}

// -----------------------------------------------------------------------------
long long Pidbm::Private::printRecord (const std::vector<std::string> & what,
                                       const std::string & from,
                                       const std::string & where,
                                       const std::vector<std::string> & condition,
                                       const std::string & orderby,
                                       const std::string & groupby) {
  long long n;
  cppdb::result records;

  n = selectRecord (records, what, from, where, condition, orderby, groupby);

  if (n > 0) {
    int len;
    std::ostringstream req;
    cppdb::result res;
    vector<int> colen;
    cppdb::statement st;

    // query to calculate the width of the columns
    req << "SELECT ";
    for (size_t i = 0; i < what.size(); i++) {
      string str (what[i]);

      if (str[0] == '%') {
        str.erase (0, 1);
      }
      req << "MAX(LENGTH(" << str << "))";
      if (i < (what.size() - 1)) {
        req << ',';
      }
    }

    req << " FROM " << from;

    if (where.size() && condition.size()) {
      req << " WHERE " << where;
      st = db << req.str();
      for (auto c : condition) {
        st << c;
      }
    }
    else {
      st = db << req.str();
    }
    //cout << req.str() << endl; // debug
    res = st.row();

    for (int i = 0; i < res.cols(); i++) {

      res.fetch (i, len);
      colen.push_back (len);
    }

    printTable (records, colen, what);
    cout << n << " records found." << endl;
  }
  else {

    cout << "No record found." << endl;
  }
  return n;
}

// -----------------------------------------------------------------------------
void Pidbm::Private::printTable (cppdb::result & res, std::vector<int> & colen,
                                 const std::vector<std::string> & what) {

  printTableHeader (res, colen, what); // call that can modify colen
  while (res.next()) {

    for (int i = 0; i < res.cols(); i++) {
      string value;

      res.fetch (i, value);
      size_t padding = colen[i] - value.size() + 1;
      cout << "| " << value << string (padding, ' ');
    }
    cout << "|" << endl;
  }
  printTableLine (colen);
}

// -----------------------------------------------------------------------------
void Pidbm::Private::printTableHeader (cppdb::result & res, std::vector<int> & colen,
                                       const std::vector<std::string> & what) {
  vector<string> colname;

  for (int i = 0; i < res.cols(); i++) {
    size_t count = 0;

    colname.push_back (columnNameCleanup (res.name (i)));

    for (auto w : what) {
      size_t pos = w.rfind (colname[i]);

      if (pos != string::npos) {

        if (pos > 0) {
          if (w[pos - 1] == '.') {
            count++;
          }
        }
        else {
          count ++;
        }
      }
    }
    if (count > 1) {

      colname[i] = what[i];
    }

    if (colname[i].size() > static_cast<size_t> (colen[i])) {
      colen[i] = colname[i].size();
    }
  }
  printTableLine (colen);

  for (int i = 0; i < res.cols(); i++) {

    colname[i].resize (colen[i], ' ');
    cout << "| " << colname[i] << ' ';
  }
  cout << "|" << endl;
  printTableLine (colen);
}

// -----------------------------------------------------------------------------
void Pidbm::Private::printTableLine (std::vector<int> & colen) {

  for (size_t i = 0; i < colen.size(); i++) {
    cout << '+' << string (colen[i] + 2, '-');
  }
  cout << '+' << endl;
}

// -----------------------------------------------------------------------------
std::string Pidbm::Private::columnNameCleanup (const std::string & name) {
  const string printHex ("printf(\"0x%x\",");
  string str (name);

  if (name.find (printHex) == 0) {

    str.erase (0, printHex.size());
    str.erase (str.rfind (")"), 1);
  }
  return str;
}

// -----------------------------------------------------------------------------
void Pidbm::Private::setWhereCondition (const std::string & arg, std::string & where,
                                        std::string & condition, bool & like) {
  string add_to_where;
  bool id = true;
  unsigned long i;

  try {
    std::size_t p = 0;

    i = std::stol (arg, &p, 0);
    if (p != arg.size()) {
      id = false;
    }
  }
  catch (...) {

    id = false;
  }

  if (id) {

    add_to_where = "id";
    condition = to_string (i);
  }
  else {

    add_to_where = "name";
    like = true;
    condition = arg;
  }
  if (where.empty()) {

    where = add_to_where;
  }
  else {
    if (where.rfind ('.') == (where.size() - 1)) {

      where = where + add_to_where;
    }
  }
}

// -----------------------------------------------------------------------------
void Pidbm::Private::setWhereCondition (size_t pos, std::string & where,
                                        std::string & condition, bool & like) {
  auto args =  op.non_option_args();

  if (args.size() > pos) {

    setWhereCondition (args[pos], where, condition, like);
  }
}

// -----------------------------------------------------------------------------
bool Pidbm::Private::readArg (size_t pos, const std::string & from,
                              long long & id, bool caseInsensitive) {
  cppdb::result res;
  string where, condition;
  bool like;
  bool found = false;

  setWhereCondition (pos + 2, where, condition, like);
  if (caseInsensitive && where == "name") {
    condition = toLower (condition);
    where = "lower(name)";
  }
  selectRecordEqual (res, {"id"}, from, where, condition);

  if (res.next()) {
    res >> id;
    found = true;
  }
  return found;
}

// -----------------------------------------------------------------------------
bool Pidbm::Private::readArg (size_t pos, const std::string & from,
                              std::string & id, bool caseInsensitive) {
  long long i;

  bool found = readArg (pos, from, i, caseInsensitive);
  if (found) {

    id = to_string (i);
  }
  return found;
}

// -----------------------------------------------------------------------------
bool Pidbm::Private::readArg (const std::string & arg, const std::string & from,
                              long long & id, bool caseInsensitive) {
  cppdb::result res;
  string where, condition;
  bool like;
  bool found = false;

  setWhereCondition (arg, where, condition, like);
  if (caseInsensitive && where == "name") {
    condition = toLower (condition);
    where = "lower(name)";
  }
  selectRecordEqual (res, {"id"}, from, where, condition);

  if (res.next()) {
    res >> id;
    found = true;
  }
  return found;
}

// -----------------------------------------------------------------------------
bool Pidbm::Private::readArg (const std::string & arg, const std::string & from,
                              std::string & id, bool caseInsensitive) {
  long long i;

  bool found = readArg (arg, from, i, caseInsensitive);
  if (found) {

    id = to_string (i);
  }
  return found;
}

// -----------------------------------------------------------------------------
long long Pidbm::Private::nameExists (const std::string & from,
                                      const std::string & name, bool caseInsensitive) {
  long long name_id = -1;
  cppdb::result records;
  string where = caseInsensitive ? "lower(name)" : "name";
  string condition = caseInsensitive ? toLower (name) : name;

  selectRecordEqual (records, {"id"}, from, where, condition);
  if (records.next()) {

    records >> name_id;
  }
  return name_id;
}

// -----------------------------------------------------------------------------
bool Pidbm::Private::idExists (const std::string & from, const std::string & id) {
  cppdb::result records;

  selectRecordEqual (records, {"id"}, from, "id", id);
  return records.next();
}

// -----------------------------------------------------------------------------
bool Pidbm::Private::idExists (const std::string & from, const long long & id) {

  return idExists (from, to_string (id));
}

// -----------------------------------------------------------------------------
void Pidbm::Private::checkDatabaseSchemaVersion() {
  int major, minor;
  cppdb::result res =
    db << "SELECT major,minor "
    "FROM schema_version "
    "WHERE valid_to IS NULL"
    <<  cppdb::row;

  if (res.empty()) {

    throw std::logic_error ("Database schema version not found");
  }
  res >> major >> minor;
  if (major != PIDUINO_DBSCHEMA_MAJOR || minor != PIDUINO_DBSCHEMA_MINOR) {

    throw std::logic_error ("Invalid database schema version, should be " +
                            to_string (PIDUINO_DBSCHEMA_MAJOR) +
                            "." + to_string (PIDUINO_DBSCHEMA_MINOR) +
                            ", version " + to_string (major) +
                            "." + to_string (minor) + " found.");
  }
}

// -----------------------------------------------------------------------------
bool
Pidbm::Private::findConnectionInfo () {

  if (cinfo.empty()) {
    const char * env;

    env = std::getenv ("PIDUINO_CONN_INFO");
    if (env) {

      cinfo.assign (env);
    }
    else {
      std::string fn;

      env = std::getenv ("HOME");
      if (env == NULL) {

        env = getpwuid (getuid())->pw_dir;
      }

      if (env) {

        fn.assign (env);
        fn += "/.config/piduino.conf";
      }

      if (!fileExists (fn)) {

        fn.assign (PIDUINO_INSTALL_ETC_DIR);
        fn += "/piduino.conf";
      }

      if (fileExists (fn)) {
        ConfigFile cfg (fn);

        cinfo = cfg.value ("connection_info");
      }
    }
  }
  return ! cinfo.empty();
}
/* ========================================================================== */
