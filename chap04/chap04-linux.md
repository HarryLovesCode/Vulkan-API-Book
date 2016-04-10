## Creating a Window on Linux

In order to create a window, we're going to be using platform specific code. Please note that this tutorial is for Linux **only**. None of this will apply to Windows, Android, GLFW, etc.

### Including Headers

For windowing on Linux, we're going to be using the **X protocol C-language Binding (XCB)**. This, while not as simple as using `GLFW` is a pretty easy API to use. However, before we can begin, we'll need to include the header:

```cpp
#include <xcb/xcb.h>
```

If you're targeting Windows as well, you should surround the include by the `#if defined(__linux__)` directives.

### Our `initWindow` Method

For this chapter, we're going to be writing the contents of these two methods:

```cpp
void VulkanExample::initWindow() {}
void VulkanExample::renderLoop() {}
```

Again, if you're targeting Windows as well, you should surround both by the `#if defined(__linux__)` directives.

### Our Variables

We'll need to keep track of a few variables in our class. I'll be adding these:

```cpp
xcb_connection_t * connection;
xcb_window_t window;
xcb_screen_t * screen;
xcb_atom_t wmProtocols;
xcb_atom_t wmDeleteWin;
```

to our `VulkanExample` class.

### Getting a Display

XCB will allow us to interact with the X server, but we'll need to get a display first. We can use the `xcb_connect` method to get the value of the `DISPLAY` environment variable. Because we don't know it already, we should pass in `NULL` for that argument. Here is the definition and usage:

```cpp
// Definition
xcb_connection_t* xcb_connect(const char * displayname, int * screenp);

// Usage
int screenp = 0;
connection = xcb_connect(NULL, &screenp);
```

Once we've called `xcb_connect`, we should check for any connection errors. We can use the `xcb_connection_has_error` method for this. The definition and usage are simply:

```cpp
// Definition
int xcb_connection_has_error(xcb_connection_t * c);

// Usage
if (xcb_connection_has_error(connection))
  exitOnError("Failed to connect to X server using XCB.");
```

Now that we've handled any errors, we need to get available screens. In order to get data from the X server, we can use `xcb_get_setup`. Then we can get a screen iterator from the setup using `xcb_setup_roots_iterator`. Once we've done that, we should loop through the screens using `xcb_screen_next`. The defintions and usage look like this:

```cpp
// Definitions
const struct xcb_setup_t* xcb_get_setup(xcb_connection_t * c);
xcb_screen_iterator_t xcb_setup_roots_iterator(const xcb_setup_t * R);
void xcb_screen_next(xcb_screen_iterator_t * i);

// Usage
xcb_screen_iterator_t iter =
    xcb_setup_roots_iterator(xcb_get_setup(connection));

for (int s = screenp; s > 0; s--)
    xcb_screen_next(&iter);
```

We can then say:

```cpp
screen = iter.data;
```

### Creating a Window

Before we can create a Window, we'll need to allocate a X Id for it. We can use the `xcb_generate_id` method for this. The definition and proper usage would be:

```cpp
// Definition
uint32_t xcb_generate_id(xcb_connection_t * c);

// Usage
window = xcb_generate_id(connection);
```

In XCB, we use **event masks** to register event types. Event masks are essentially **bit masks** which will tell XCB what events we care about. We'll be using two values for our event mask: `XCB_CW_BACK_PIXEL` and `XCB_CW_EVENT_MASK`. The first will tell XCB we're going to be filling the background of the window with one pixel color. The second is required. We're going to then use a bitwise `OR` operator (`|`) on them:

```cpp
uint32_t eventMask = XCB_CW_BACK_PIXEL | XCB_CW_EVENT_MASK;
```

Now we can prepare the values that will get sent over to XCB. This will include the background color we want and the events we're looking for:

```cpp
uint32_t valueList[] = {screen->black_pixel, 0};
```

For now, we're not going to worry about events such as keypresses. Now we can use the `xcb_create_window` method to create our window:

```cpp
// Definition
xcb_void_cookie_t xcb_create_window(
  xcb_connection_t * c,
  uint8_t           depth,
  xcb_window_t      window,
  xcb_window_t      parent,
  int16_t           x,
  int16_t           y,
  uint16_t          width,
  uint16_t          height,
  uint16_t          border_width,
  uint16_t          class,
  xcb_visualid_t    visual,
  uint32_t          value_mask,
  const uint32_t *  value_list);

// Usage
xcb_create_window(
  connection,
  XCB_COPY_FROM_PARENT,
  window,
  screen->root,
  0,
  0,
  windowWidth,
  windowHeight,
  0,
  XCB_WINDOW_CLASS_INPUT_OUTPUT,
  screen->root_visual,
  eventMask,
  valueList);
```

Before we map the window to the screen, we're going to change the title. We can use the `xcb_change_property` method to do this:

```cpp
// Definition
xcb_void_cookie_t xcb_change_property(
  xcb_connection_t * conn,
  uint8_t            mode,
  xcb_window_t       window,
  xcb_atom_t         property,
  xcb_atom_t         type,
  uint8_t            format,
  uint32_t           data_len,
  const void *       data);
// Usage
xcb_change_property(
  connection,
  XCB_PROP_MODE_REPLACE,
  window,
  XCB_ATOM_WM_NAME,
  XCB_ATOM_STRING,
  8,
  strlen(applicationName),
  applicationName);
```

Now we can simply use `xcb_map_window` and `xcb_flush` to map the window to the screen and flush the stream to the server:

```cpp
// Definition
xcb_void_cookie_t xcb_map_window(
  xcb_connection_t * conn,
  xcb_window_t       window);
// Usage
xcb_map_window(connection, window);
xcb_flush(connection);
```

### Close Button

I won't go too far into depth about the code below. Essentially what it does is tells the server we want to be alerted when the window manager attempts to destroy the window:

```cpp
xcb_intern_atom_cookie_t wmDeleteCookie = xcb_intern_atom(
    connection, 0, strlen("WM_DELETE_WINDOW"), "WM_DELETE_WINDOW");
xcb_intern_atom_cookie_t wmProtocolsCookie =
    xcb_intern_atom(connection, 0, strlen("WM_PROTOCOLS"), "WM_PROTOCOLS");
xcb_intern_atom_reply_t *wmDeleteReply =
    xcb_intern_atom_reply(connection, wmDeleteCookie, NULL);
xcb_intern_atom_reply_t *wmProtocolsReply =
    xcb_intern_atom_reply(connection, wmProtocolsCookie, NULL);
wmDeleteWin = wmDeleteReply->atom;
wmProtocols = wmProtocolsReply->atom;

xcb_change_property(connection, XCB_PROP_MODE_REPLACE, window,
                    wmProtocolsReply->atom, 4, 32, 1, &wmDeleteReply->atom);
```

That piece of code goes right before we map the window to the screen and flush the stream.

### Update Loop

All we need to do now is write the contents of the `renderLoop` method. Again, I'm not going to be going too far into depth because this is just glue and we're focused on Vulkan here. Anyways, we'll be using the `xcb_wait_for_event` method to do just that: wait for events. In order to convert the server's response to a value we can check in a **switch case** block, we need to use the bitwise `AND` (`&`) operator with the value: `~0x80`. You can see what I'm talking about below:

```cpp
void VulkanExample::renderLoop() {
  bool running = true;

  while (running) {
    xcb_generic_event_t *event = xcb_wait_for_event(connection);

    switch (event->response_type & ~0x80) {
      case XCB_CLIENT_MESSAGE: {
        xcb_client_message_event_t *cm = (xcb_client_message_event_t *)event;
        if (cm->data.data32[0] == wmDeleteWin)
          running = false;
        break;
      }
    }
    free(event);
  }

  xcb_destroy_window(connection, window);
}
```

Once we get the `wmDeleteWin` response, we should use `xcb_destroy_window` to destroy our main window.
