## Creating a Window on Linux

In order to create a window, we're going to be using platform specific code. Please note that this tutorial is for Linux **only**. None of this will apply to Windows, Android, GLFW, etc.

### Including Headers

For windowing on Linux, we're going to be using the **X protocol C-language Binding (XCB)**. This, while not as simple as using GLFW is still a pretty easy API to use. Before we can begin, we'll need to include the header:

```cpp
#include <xcb/xcb.h>
```

If you're targeting Windows as well, you should surround the include by the `#if defined(__linux__)` directives.

### Methods in Our Class

For this chapter, we're going to be writing the contents of these two methods:

```cpp
void createWindow() {}
void renderLoop() {}
```

Again, if you're targeting Windows as well, you should surround both by the `#if defined(__linux__)` directives.

### Variables in Our Class

We'll need to keep track of a few variables in our class. I'll be adding these:

```cpp
xcb_connection_t * connection;
xcb_window_t window;
xcb_screen_t * screen;
xcb_atom_t wmProtocols;
xcb_atom_t wmDeleteWin;
```

to our `VulkanExample` class.

### Getting a Connection

XCB will allow us to interact with the X server, but we'll need to get a display first. We can use the `xcb_connect` method to get the value of the `DISPLAY` environment variable. Because we don't know it already, we should pass in `NULL` for that argument.

**Definition for `xcb_connect`**:

```cpp
xcb_connection_t* xcb_connect(const char * displayname, int * screenp);
```

**[Documentation](https://xcb.freedesktop.org/manual/group__XCB__Core__API.html#ga094470586356d1764e69c9a1882966c3) for `xcb_connect`**:

- `displayname` is the name of the display or `NULL` if we want the display specified by the `DISPLAY` variable.
- `screenp` is a pointer to a preferred screen number.
- Returns a newly allocated `xcb_connection_t` structure.

**Usage for `xcb_connect`**:

```cpp
int screenp = 0;
connection = xcb_connect(NULL, &screenp);
```

**Definition for `xcb_connection_has_error`**:

Once we've called `xcb_connect`, we should check for any connection errors. It is recommended that we use the `xcb_connection_has_error` method for this.

```cpp
int xcb_connection_has_error(xcb_connection_t * c);
```

**[Documentation](https://xcb.freedesktop.org/manual/group__XCB__Core__API.html#ga70a6bade94bd2824db552abcf5fbdbe3) for `xcb_connection_has_error`**:

- `c` is the connection.
- Returns a value > `0` if the connection is in an error state; `0` otherwise.

**Usage for `xcb_connection_has_error`**:

```cpp
if (xcb_connection_has_error(connection))
  exitOnError("Failed to connect to X server using XCB.");
```

### Getting Setups

Now that we've handled any errors, we need to get available screens. In order to get data from the X server, we can use `xcb_get_setup`. Then we can get a screen iterator from the setup using `xcb_setup_roots_iterator`. Once we've done that, we should loop through the screens using `xcb_screen_next`.

**Definition for `xcb_get_setup`**:

```cpp
const struct xcb_setup_t* xcb_get_setup(xcb_connection_t * c);
```

**[Documentation](https://xcb.freedesktop.org/manual/group__XCB__Core__API.html#gaff17d9fb3ea142975ca225a89ac2a4df) for `xcb_get_setup`**:

- `c`	is the connection.
- Returns a pointer to an `xcb_setup_t` structure.

**Definition for `xcb_setup_roots_iterator`**:

```cpp
xcb_screen_iterator_t xcb_setup_roots_iterator(const xcb_setup_t * R);
```

**[Documentation]() for `xcb_setup_roots_iterator`**:

- TODO

**Definition for `xcb_screen_next`**:

```cpp
void xcb_screen_next(xcb_screen_iterator_t * i); 	
```

**[Documentation](https://xcb.freedesktop.org/manual/group__XCB____API.html#ga59cff0d119ca040f77e24510f15445e1) for `xcb_screen_next`**:

- `i`	is a pointer to a `xcb_screen_iterator_t`.

**Usage for all methods**:

```cpp
xcb_screen_iterator_t iter =
    xcb_setup_roots_iterator(xcb_get_setup(connection));

for (int s = screenp; s > 0; s--)
    xcb_screen_next(&iter);
```

Once we have looped through all of the screens, we can then say:

```cpp
screen = iter.data;
```

### Generating a XID

Before we can create a Window, we'll need to allocate a XID for it. We can use the `xcb_generate_id` method for this.

**Definition for `xcb_generate_id`**:

```cpp
uint32_t xcb_generate_id(xcb_connection_t * c);
```

**[Documentation](https://xcb.freedesktop.org/manual/group__XCB__Core__API.html#ga3208afc306e3c564435a6ecbaddf80d1) for `xcb_generate_id`**:

- `c` is the connection.
- Returns a newly allocated XID.

**Usage for `xcb_generate_id`**:

```cpp
window = xcb_generate_id(connection);
```

### Event Masks

In XCB, we use **event masks** to register event types. Event masks are essentially **bitmasks** which will tell XCB what events we care about. We'll be using two values for our event mask: `XCB_CW_BACK_PIXEL` and `XCB_CW_EVENT_MASK`. The first will tell XCB we're going to be filling the background of the window with one pixel color. The second is required. We're going to then use a bitwise `OR` operator to create the bitmask.

```cpp
uint32_t eventMask = XCB_CW_BACK_PIXEL | XCB_CW_EVENT_MASK;
```

Now we can prepare the values that will get sent over to XCB. This will include the background color we want and the events we're looking for:

```cpp
uint32_t valueList[] = { screen->black_pixel, 0 };
```

For now, we're not going to worry about events such as keypresses. So, we can go ahead and use the `xcb_create_window` method to create our window.

**Definition for `xcb_create_window`**:

```cpp
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
```

**[Documentation](http://www.x.org/archive/current/doc/man/man3/xcb_create_window.3.xhtml) for `xcb_create_window`**:

- `conn` is the XCB connection to X11.
- `depth` specifies the new window’s depth in units. The special value `XCB_COPY_FROM_PARENT `means the depth is taken from the parent window.
- `wid` is the ID with which you will refer to the new window, created by `xcb_generate_id`.
- `parent` is the parent window of the new window.
- `x` is the X coordinate of the new window.
- `y` is the Y coordinate of the new window.
- `width` is the width of the new window.
- `height` is the height of the new window.
- `border_width` must be zero if the class is `InputOnly` or a `xcb_match_error_t` occurs.
- `class` must be one of the following values: `XCB_WINDOW_CLASS_COPY_FROM_PARENT`, `XCB_WINDOW_CLASS_INPUT_OUTPUT`, or `XCB_WINDOW_CLASS_INPUT_ONLY`.
- `visual` specifies the id for the new window’s visual.
- Returns a `xcb_void_cookie_t`. Errors (if any) have to be handled in the event loop.

**Usage for `xcb_create_window`**:

```cpp
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

### Modifying Properties

Before we map the window to the screen, we're going to change the title. We can use the `xcb_change_property` method to do this.

**Definition for `xcb_change_property`**:

```cpp
xcb_void_cookie_t xcb_change_property(
  xcb_connection_t * conn,
  uint8_t            mode,
  xcb_window_t       window,
  xcb_atom_t         property,
  xcb_atom_t         type,
  uint8_t            format,
  uint32_t           data_len,
  const void *       data);
```

**[Documentation]() for `xcb_change_property`**:

- `conn` is the XCB connection to X11.
- `mode` is one of the following values: `XCB_PROP_MODE_REPLACE` (Discard the previous property value and store the new data), `XCB_PROP_MODE_PREPEND` (Insert the new data before the beginning of existing data), or `XCB_PROP_MODE_APPEND` (Insert the new data after the beginning of existing data).
- `window` is the window whose property you want to change.
- `property` is the property you want to change (an atom).
- `type` is the type of the property you want to change (an atom).
- `format` specifies whether the data should be viewed as a list of 8-bit, 16-bit or 32-bit quantities. Possible values are 8, 16 and 32.
- `data_len` specifies the number of elements (see format).
- `data` is the property data.
- Returns an `xcb_void_cookie_t`. Errors (if any) have to be handled in the event loop.

**Usage for `xcb_change_property`**:

```cpp
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

### Window Mapping and Event Stream Flushing

Now we can simply use `xcb_map_window` and `xcb_flush` to map the window to the screen and flush the stream to the server.

**Definition for `xcb_map_window`**:

```cpp
xcb_void_cookie_t xcb_map_window(
  xcb_connection_t * conn,
  xcb_window_t       window);
```

**[Documentation](http://www.x.org/archive/current/doc/man/man3/xcb_map_window.3.xhtml) for `xcb_map_window`**:

- `conn` is the XCB connection to X11.
- `window` is the window to make visible.
- Returns an `xcb_void_cookie_t`. Errors (if any) have to be handled in the event loop.

**Definition for `xcb_flush`**:

```cpp
int xcb_flush(xcb_connection_t * c); 	
```

**[Documentation](https://xcb.freedesktop.org/manual/group__XCB__Core__API.html#ga9361ead09eb756ba9cc8953c4569a789) for `xcb_flush`**:

- `c` is the connection to the X server.
- Returns > `0` on success, <= `0` otherwise.

**Usage for both methods**:

```cpp
xcb_map_window(connection, window);
xcb_flush(connection);
```

### Close Button

I won't go too far into depth about the code below. Essentially what it does is tells the server we want to be alerted when the window manager attempts to destroy the window. This piece of code goes right before we map the window to the screen and flush the stream:

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

### Our Update Loop

All we need to do now is write the contents of the `renderLoop` method. Again, I'm not going to be going too far into depth because this is just glue and we're focused on Vulkan here. Anyways, we'll be using the `xcb_wait_for_event` method to do just that: wait for events. In order to convert the server's response to a value we can check in a **switch case** block, we need to use the bitwise `AND` (`&`) operator with the value: `~0x80`. You can see what I'm talking about below:

```cpp
void VulkanExample::renderLoop() {
  bool running = true;
  xcb_generic_event_t *event;
  xcb_client_message_event_t *cm;

  while (running) {
    event = xcb_wait_for_event(connection);

    switch (event->response_type & ~0x80) {
      case XCB_CLIENT_MESSAGE: {
        cm = (xcb_client_message_event_t *)event;

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
