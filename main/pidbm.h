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
#include <pimp.h>
#include <popl.h>

std::string progName();

class Pidbm {

  public:

    Pidbm ();
    virtual ~Pidbm();

    int parse (int argc, char **argv);
    bool open ();
    void close();
    bool isOpen() const;
    void exec ();
    
    void help (std::ostream & os = std::cout) const;

    static void version();
    static void warranty();

  protected:
    class Private;
    Pidbm (Private &dd);
    std::unique_ptr<Private> d_ptr;

  private:
    PIMP_DECLARE_PRIVATE (Pidbm)
};

/* ========================================================================== */
