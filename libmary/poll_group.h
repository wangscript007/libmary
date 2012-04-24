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


#ifndef __LIBMARY__POLL_GROUP__H__
#define __LIBMARY__POLL_GROUP__H__


#include <libmary/libmary_config.h>
#include <libmary/types.h>
#include <libmary/cb.h>
#include <libmary/exception.h>
#include <libmary/deferred_processor.h>


namespace M {

class PollGroup
{
public:
    enum EventFlags {
	Input  = 0x1,
	Output = 0x2,
	Error  = 0x4,
	Hup    = 0x8
    };

    struct Feedback {
	void (*requestInput)  (void *cb_data);
	void (*requestOutput) (void *cb_data);
    };

    struct Pollable
    {
	void (*processEvents) (Uint32  event_flags,
			       void   *cb_data);

#ifdef PLATFORM_WIN32
	// TODO HANDLE?
	int (*getFd) (void *cb_data);
#else
	int (*getFd) (void *cb_data);
#endif

	void (*setFeedback) (Cb<Feedback> const &feedback,
			     void *cb_data);
    };

    typedef void *PollableKey;

    // Every successful call to addPollable() must be matched with a call
    // to removePollable().
    virtual mt_throws PollableKey addPollable (CbDesc<Pollable> const &pollable,
					       DeferredProcessor::Registration *ret_reg,
					       bool activate = true) = 0;

    virtual mt_throws Result activatePollable (PollableKey mt_nonnull key) = 0;

    virtual void removePollable (PollableKey mt_nonnull key) = 0;

    virtual ~PollGroup () {}
};

}


namespace M {
    // TODO Rename DefaultPollGroup to DefaultActivePollGroup
    //      and move this into active_poll_group.h
#if defined (LIBMARY_USE_POLL)
    class PollPollGroup;
    typedef PollPollGroup DefaultPollGroup;
#elif defined (LIBMARY_USE_SELECT) || !defined (PLATFORM_DEFAULT)
    class SelectPollGroup;
    typedef SelectPollGroup DefaultPollGroup;
#else
    class EpollPollGroup;
    typedef EpollPollGroup DefaultPollGroup;
#endif
}
#include <libmary/active_poll_group.h>


#endif /* __LIBMARY__POLL_GROUP__H__ */

