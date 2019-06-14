/*
 * Copyright © 2008 Kristian Høgsberg
 *
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that copyright
 * notice and this permission notice appear in supporting documentation, and
 * that the name of the copyright holders not be used in advertising or
 * publicity pertaining to distribution of the software without specific,
 * written prior permission.  The copyright holders make no representations
 * about the suitability of this software for any purpose.  It is provided "as
 * is" without express or implied warranty.
 *
 * THE COPYRIGHT HOLDERS DISCLAIM ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO
 * EVENT SHALL THE COPYRIGHT HOLDERS BE LIABLE FOR ANY SPECIAL, INDIRECT OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE,
 * DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
 * TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE
 * OF THIS SOFTWARE.
 */

#ifndef WAYLAND_CLIENT_H
#define WAYLAND_CLIENT_H

#include "wayland-util.h"
#include "wayland-version.h"

#ifdef  __cplusplus
extern "C" {
#endif

/** \class wl_proxy
 *
 * \brief Represents a protocol object on the client side.
 *
 * A wl_proxy acts as a client side proxy to an object existing in the
 * compositor. The proxy is responsible for converting requests made by the
 * clients with \ref wl_proxy_marshal() into Wayland's wire format. Events
 * coming from the compositor are also handled by the proxy, which will in
 * turn call the handler set with \ref wl_proxy_add_listener().
 *
 * \note With the exception of function \ref wl_proxy_set_queue(), functions
 * accessing a wl_proxy are not normally used by client code. Clients
 * should normally use the higher level interface generated by the scanner to
 * interact with compositor objects.
 *
 */
struct wl_proxy;

/** \class wl_display
 *
 * \brief Represents a connection to the compositor and acts as a proxy to
 * the wl_display singleton object.
 *
 * A wl_display object represents a client connection to a Wayland
 * compositor. It is created with either \ref wl_display_connect() or
 * \ref wl_display_connect_to_fd(). A connection is terminated using
 * \ref wl_display_disconnect().
 *
 * A wl_display is also used as the \ref wl_proxy for the wl_display
 * singleton object on the compositor side.
 *
 * A wl_display object handles all the data sent from and to the
 * compositor. When a \ref wl_proxy marshals a request, it will write its wire
 * representation to the display's write buffer. The data is sent to the
 * compositor when the client calls \ref wl_display_flush().
 *
 * Incoming data is handled in two steps: queueing and dispatching. In the
 * queue step, the data coming from the display fd is interpreted and
 * added to a queue. On the dispatch step, the handler for the incoming
 * event set by the client on the corresponding \ref wl_proxy is called.
 *
 * A wl_display has at least one event queue, called the <em>default
 * queue</em>. Clients can create additional event queues with \ref
 * wl_display_create_queue() and assign \ref wl_proxy's to it. Events
 * occurring in a particular proxy are always queued in its assigned queue.
 * A client can ensure that a certain assumption, such as holding a lock
 * or running from a given thread, is true when a proxy event handler is
 * called by assigning that proxy to an event queue and making sure that
 * this queue is only dispatched when the assumption holds.
 *
 * The default queue is dispatched by calling \ref wl_display_dispatch().
 * This will dispatch any events queued on the default queue and attempt
 * to read from the display fd if it's empty. Events read are then queued
 * on the appropriate queues according to the proxy assignment.
 *
 * A user created queue is dispatched with \ref wl_display_dispatch_queue().
 * This function behaves exactly the same as wl_display_dispatch()
 * but it dispatches given queue instead of the default queue.
 *
 * A real world example of event queue usage is Mesa's implementation of
 * eglSwapBuffers() for the Wayland platform. This function might need
 * to block until a frame callback is received, but dispatching the default
 * queue could cause an event handler on the client to start drawing
 * again. This problem is solved using another event queue, so that only
 * the events handled by the EGL code are dispatched during the block.
 *
 * This creates a problem where a thread dispatches a non-default
 * queue, reading all the data from the display fd. If the application
 * would call \em poll(2) after that it would block, even though there
 * might be events queued on the default queue. Those events should be
 * dispatched with \ref wl_display_dispatch_(queue_)pending() before
 * flushing and blocking.
 */
struct wl_display;

/** \class wl_event_queue
 *
 * \brief A queue for \ref wl_proxy object events.
 *
 * Event queues allows the events on a display to be handled in a thread-safe
 * manner. See \ref wl_display for details.
 *
 */
struct wl_event_queue;

void wl_event_queue_destroy(struct wl_event_queue *queue);

void wl_proxy_marshal(struct wl_proxy *p, uint32_t opcode, ...);
void wl_proxy_marshal_array(struct wl_proxy *p, uint32_t opcode,
			    union wl_argument *args);
struct wl_proxy *wl_proxy_create(struct wl_proxy *factory,
				 const struct wl_interface *interface);
struct wl_proxy *wl_proxy_marshal_constructor(struct wl_proxy *proxy,
					      uint32_t opcode,
					      const struct wl_interface *interface,
					      ...);
struct wl_proxy *
wl_proxy_marshal_array_constructor(struct wl_proxy *proxy,
				   uint32_t opcode, union wl_argument *args,
				   const struct wl_interface *interface);

void wl_proxy_destroy(struct wl_proxy *proxy);
int wl_proxy_add_listener(struct wl_proxy *proxy,
			  void (**implementation)(void), void *data);
const void *wl_proxy_get_listener(struct wl_proxy *proxy);
int wl_proxy_add_dispatcher(struct wl_proxy *proxy,
			    wl_dispatcher_func_t dispatcher_func,
			    const void * dispatcher_data, void *data);
void wl_proxy_set_user_data(struct wl_proxy *proxy, void *user_data);
void *wl_proxy_get_user_data(struct wl_proxy *proxy);
uint32_t wl_proxy_get_id(struct wl_proxy *proxy);
const char *wl_proxy_get_class(struct wl_proxy *proxy);
void wl_proxy_set_queue(struct wl_proxy *proxy, struct wl_event_queue *queue);

#include "wayland-client-protocol.h"

struct wl_display *wl_display_connect(const char *name);
struct wl_display *wl_display_connect_to_fd(int fd);
void wl_display_disconnect(struct wl_display *display);
int wl_display_get_fd(struct wl_display *display);
int wl_display_dispatch(struct wl_display *display);
int wl_display_dispatch_queue(struct wl_display *display,
			      struct wl_event_queue *queue);
int wl_display_dispatch_queue_pending(struct wl_display *display,
				      struct wl_event_queue *queue);
int wl_display_dispatch_pending(struct wl_display *display);
int wl_display_get_error(struct wl_display *display);
uint32_t wl_display_get_protocol_error(struct wl_display *display,
				       const struct wl_interface **interface,
				       uint32_t *id);

int wl_display_flush(struct wl_display *display);
int wl_display_roundtrip_queue(struct wl_display *display,
                               struct wl_event_queue *queue);
int wl_display_roundtrip(struct wl_display *display);
struct wl_event_queue *wl_display_create_queue(struct wl_display *display);

int wl_display_prepare_read_queue(struct wl_display *display,
				  struct wl_event_queue *queue);
int wl_display_prepare_read(struct wl_display *display);
void wl_display_cancel_read(struct wl_display *display);
int wl_display_read_events(struct wl_display *display);

void wl_log_set_handler_client(wl_log_func_t handler);

#ifdef  __cplusplus
}
#endif

#endif
