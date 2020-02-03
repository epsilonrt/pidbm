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
#include <exception>
#include <iomanip>
#include <algorithm>
#include <sstream>
#include "soc.h"

using namespace std;

// -----------------------------------------------------------------------------
//
//                              Arch Class
//
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
Arch::Arch (cppdb::session & d, long long i) : _db (d), _id (i) {

  if (i >= 0) {
    setId (i);
  }
}

// -----------------------------------------------------------------------------
void Arch::setId (long long i) {

  cppdb::result res =
    _db << "SELECT name "
    "FROM arch "
    "WHERE id=?"
    << i << cppdb::row;

  if (res.empty()) {

    throw std::invalid_argument ("Arch not found");
  }
  _id = i;
  res >> _name;
}


// -----------------------------------------------------------------------------
//
//                          Manufacturer Class
//
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
Manufacturer::Manufacturer (cppdb::session & d, long long i) : _db (d), _id (i) {

  if (i >= 0) {
    setId (i);
  }
}

// -----------------------------------------------------------------------------
void Manufacturer::setId (long long i) {

  cppdb::result res =
    _db << "SELECT name "
    "FROM manufacturer "
    "WHERE id=?"
    << i << cppdb::row;

  if (res.empty()) {

    throw std::invalid_argument ("Manufacturer not found");
  }
  _id = i;
  res >> _name;
}

// -----------------------------------------------------------------------------
//
//                     Soc::Family Class
//
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
Soc::Family::Family (cppdb::session & d, long long i) : _db (d), _id (i), _arch (d) {

  if (i >= 0) {
    setId (i);
  }
}

// -----------------------------------------------------------------------------
void Soc::Family::setId (long long i) {

  cppdb::result res =
    _db << "SELECT name,arch_id "
    "FROM soc_family "
    "WHERE id=?"
    << i << cppdb::row;
  long long arch_id;

  if (res.empty()) {

    throw std::invalid_argument ("Soc Family not found");
  }
  _id = i;
  res >> _name >> arch_id;
  _arch.setId (arch_id);
}

// -----------------------------------------------------------------------------
//
//                         Soc Class
//
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
Soc::Soc (cppdb::session & d, long long i) : _db (d), _id (i),
  _family (d), _manufacturer (d), _i2c_count (0), _spi_count (0), _uart_count (0) {

  if (i >= 0) {
    setId (i);
  }
}

// -----------------------------------------------------------------------------
Soc::Soc (const Soc & src, const std::string & n) :
  _db (src._db), _family (src._db, src._family.id()),
  _manufacturer (src._db, src._manufacturer.id()), _i2c_count (src._i2c_count),
  _spi_count (src._spi_count), _uart_count (src._uart_count), _name (n) {
  cppdb::statement st;
  cppdb::result res;

  st = _db << "INSERT INTO soc(name,soc_family_id,manufacturer_id,i2c_count,"
       "spi_count,uart_count) VALUES(?,?,?,?,?,?)"
       << _name << _family.id() << _manufacturer.id() << _i2c_count
       << _spi_count << _uart_count;
  st.exec();
  _id = st.last_insert_id();

  res = _db << "SELECT pin_id FROM soc_has_pin WHERE soc_id=?" << src._id;

  while (res.next()) {
    long long pin_id;

    res >> pin_id;
    st = _db << "INSERT INTO soc_has_pin(soc_id,pin_id) VALUES(?,?)"
         << _id << pin_id;
    st.exec();
  }
}

// -----------------------------------------------------------------------------
void Soc::setId (long long i) {
  long long sfid, mid;
  cppdb::result res =
    _db << "SELECT name,soc_family_id,manufacturer_id,i2c_count,spi_count,uart_count "
    "FROM soc "
    "WHERE id=?"
    << i << cppdb::row;

  if (res.empty()) {

    throw std::invalid_argument ("Soc not found");
  }

  res >> _name >> sfid >> mid >> _i2c_count >> _spi_count >> _uart_count;
  _family.setId (sfid);
  _manufacturer.setId (mid);
}
/* ========================================================================== */
