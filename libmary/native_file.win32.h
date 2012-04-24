/*  LibMary - C++ library for high-performance network servers
    Copyright (C) 2011 Dmitry Shatrov

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/


#ifndef __LIBMARY__NATIVE_FILE_WIN32__H__
#define __LIBMARY__NATIVE_FILE_WIN32__H__


#include <libmary/file.h>


namespace M {

class NativeFile : public File
{
private:

public:
    mt_iface (File)

      mt_iface (InputStream)
	mt_throws IoResult read (Memory  mem,
				 Size   *ret_nread);
      mt_iface_end

      mt_iface (OutputStream)
        mt_throws Result write (ConstMemory  mem,
				Size        *ret_nwritten);

        mt_throws Result flush ();
      mt_iface_end

      mt_throws Result seek (FileOffset offset,
			     SeekOrigin origin);

      mt_throws Result tell (FileSize *ret_pos);

      mt_throws Result sync ();

      mt_throws Result close (bool flush_data = true);

    mt_iface_end

//    mt_throws Result stat (FileStat * mt_nonnull ret_stat);

    // Resets fd so that it won't be closed in the destructor.
    void resetFd ();

    mt_throws Result open (ConstMemory filename,
			   Uint32      open_flags,
			   AccessMode  access_mode);

    NativeFile ();

    ~NativeFile ();
};

}


#endif /* __LIBMARY__NATIVE_FILE_WIN32__H__ */

