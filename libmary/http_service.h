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


#ifndef __LIBMARY__HTTP_SERVICE__H__
#define __LIBMARY__HTTP_SERVICE__H__


#include <libmary/types.h>
#include <libmary/string_hash.h>
#include <libmary/code_referenced.h>
#include <libmary/timers.h>
#include <libmary/tcp_server.h>
#include <libmary/poll_group.h>
#include <libmary/immediate_connection_sender.h>
#include <libmary/connection_receiver.h>
#include <libmary/http.h>


namespace M {

class HttpService : public DependentCodeReferenced
{
public:
    struct HttpHandler
    {
	// If the module has subscribed to request message body pre-assembly,
	// then @msg_body points to message body.
	Result (*httpRequest) (HttpRequest   * mt_nonnull req,
			       Sender        * mt_nonnull conn_sender,
			       Memory const  &msg_body,
			       void         ** mt_nonnull ret_msg_data,
			       void          *cb_data);

	// If @req is NULL, then we have not received the request in full due to
	// an error. Such last call is made to give the module a chance
	// to release msg_data.
	Result (*httpMessageBody) (HttpRequest  * mt_nonnull req,
				   Sender       * mt_nonnull conn_sender,
				   Memory const &mem,
				   Size         * mt_nonnull ret_accepted,
				   void         *msg_data,
				   void         *cb_data);
    };

private:
    class HandlerEntry
    {
    public:
	mt_const Cb<HttpHandler> cb;

	mt_const bool preassembly;
	mt_const Size preassembly_limit;
	mt_const bool parse_body_params;

	HandlerEntry (CbDesc<HttpHandler> const &cb,
		      bool                const preassembly,
		      Size                const preassembly_limit,
		      bool                const parse_body_params)
	    : cb (cb),
	      preassembly (preassembly),
	      preassembly_limit (preassembly_limit),
	      parse_body_params (parse_body_params)
	{
	}

	HandlerEntry ()
	{
	}
    };

    class Namespace : public BasicReferenced
    {
    public:
	// We use Ref<Namespace> because 'Namespace' is an incomplete type here
	// (language limitation).
	typedef StringHash< Ref<Namespace> > NamespaceHash;
	typedef StringHash<HandlerEntry> HandlerHash;

	NamespaceHash namespace_hash;
	HandlerHash handler_hash;
    };

    class HttpConnection : public Object,
			   public IntrusiveListElement<>
    {
    public:
	bool valid;

	WeakCodeRef weak_http_service;
	HttpService *unsafe_http_service;

	TcpConnection tcp_conn;
	ImmediateConnectionSender conn_sender;
	ConnectionReceiver conn_receiver;
	HttpServer http_server;

	PollGroup::PollableKey pollable_key;
	mt_const Timers::TimerKey conn_keepalive_timer;

	// The following fields are synchroinzed by http_serverer.
	// They should only be accessed from HttpServer::Frontend callbacks.
	// {
	    HandlerEntry *cur_handler;
	    void *cur_msg_data;

	    Byte *preassembly_buf;
	    Size preassembly_buf_size;
	    Size preassembled_len;
	// }

	HttpConnection ();

	~HttpConnection ();
    };

    PollGroup *poll_group;
    Timers    *timers;
    PagePool  *page_pool;

    Time keepalive_timeout_microsec;

    mt_const bool no_keepalive_conns;

    TcpServer tcp_server;

    typedef IntrusiveList<HttpConnection> ConnectionList;
    ConnectionList conn_list;

    Namespace root_namespace;

    StateMutex mutex;

    mt_mutex (mutex) void releaseHttpConnection (HttpConnection * mt_nonnull http_conn);
    mt_mutex (mutex) void destroyHttpConnection (HttpConnection * mt_nonnull http_conn);

    static void connKeepaliveTimerExpired (void *_http_conn);

    static void doCloseHttpConnection (HttpConnection *http_conn);

  mt_iface (HttpServer::Frontend)
    static HttpServer::Frontend const http_frontend;

    static void httpRequest (HttpRequest * mt_nonnull req,
			     void        *cb_data);

    static void httpMessageBody (HttpRequest * mt_nonnull req,
				 Memory const &mem,
				 Size        * mt_nonnull ret_accepted,
				 void        *cb_data);

    static void httpClosed (Exception *exc_,
			    void      *cb_data);
  mt_iface_end()

  mt_iface (Sender::Frontend)
    static Sender::Frontend const sender_frontend;

// TODO
//    void senderStateChanged (Sender::SendState  send_state,
//			     void              *_http_conn);

    static void senderClosed (Exception *exc_,
			      void      *_http_conn);
  mt_iface_end()

    bool acceptOneConnection ();

  mt_iface (TcpServer::Frontend)
    static TcpServer::Frontend const tcp_server_frontend;

    static void accepted (void *_self);
  mt_iface_end()

    mt_mutex (mutex) void addHttpHandler_rec (CbDesc<HttpHandler> const &cb,
					      ConstMemory  path,
					      bool         preassembly,
					      Size         preassembly_limit,
					      bool         parse_body_params,
					      Namespace   *nsp);

public:
    void addHttpHandler (CbDesc<HttpHandler> const &cb,
			 ConstMemory path,
			 bool        preassembly       = false,
			 Size        preassembly_limit = 0,
			 bool        parse_body_params = false);

    mt_throws Result bind (IpAddress const &addr);

    mt_throws Result start ();

    mt_throws Result init (PollGroup * mt_nonnull poll_group,
			   Timers    * mt_nonnull timers,
			   PagePool  * mt_nonnull page_pool,
			   Time       keeaplive_timeout_microsec,
                           bool       no_keeaplive_conns);

    HttpService (Object *coderef_container);

    ~HttpService ();
};

}


#endif /* __LIBMARY__HTTP_SERVICE__H__ */

