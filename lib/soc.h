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
#pragma once

#include <string>
#include <iostream>
#include <cppdb/frontend.h>

class Arch {
  public:
    Arch (cppdb::session & db, long long id = -1);
    void setId (long long id);

    inline long long id() const {
      return _id;
    }
    inline const std::string & name() const {
      return _name;
    }
  private:
    cppdb::session & _db;
    long long _id;
    std::string _name;
};

class Manufacturer {
  public:
    Manufacturer (cppdb::session & db, long long id = -1);
    void setId (long long id);

    inline long long id() const {
      return _id;
    }
    inline const std::string & name() const {
      return _name;
    }
  private:
    cppdb::session & _db;
    long long _id;
    std::string _name;
};

class Soc {
  public:

    class Family {
      public:
        Family (cppdb::session & db, long long id = -1);
        void setId (long long id);

        inline long long id() const {
          return _id;
        }
        inline const Arch & arch() const {
          return _arch;
        }
        inline const std::string & name() const {
          return _name;
        }
        inline cppdb::session & db()  {
          return _db;
        }
      private:
        cppdb::session & _db;
        long long _id;
        std::string _name;
        Arch _arch;
    };

    Soc (cppdb::session & db, long long id = -1);
    Soc (const Soc & src, const std::string & name);

    void setId (long long id);

    inline long long id() const {
      return _id;
    }
    inline const Family & family() const {
      return _family;
    }
    inline const Manufacturer & manufacturer() const {
      return _manufacturer;
    }
    inline int i2cCount() const {
      return _i2c_count;
    }
    inline int spiCount() const {
      return _spi_count;
    }
    inline int uartCount() const {
      return _uart_count;
    }
    inline const std::string & name() const {
      return _name;
    }
    inline cppdb::session & db() const  {
      return _db;
    }

  private:
    cppdb::session & _db;
    long long _id;
    Family _family;
    Manufacturer _manufacturer;
    int _i2c_count;
    int _spi_count;
    int _uart_count;
    std::string _name;
};
/* ========================================================================== */
