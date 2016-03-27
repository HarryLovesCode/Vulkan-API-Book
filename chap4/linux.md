# Creating a Window on Linux

In order to create a window, we're going to be using platform specific code. Please note that this tutorial is for Linux **only**. None of this will apply to Windows, Android, GLFW, etc.

# Including Headers

For windowing on Linux, we're going to be using the **X protocol C-language Binding (XCB)**. This, while not as simple as using `GLFW` is a pretty easy API to use. However, before we can begin, we'll need to include the header:

```cpp
#include <xcb/xcb.h>
```

If you're targeting Windows as well, you should surround that by the `#if defined(__linux__)` directives.

# Our `initWindow` Method

For this chapter, we're going to be writing the contents of this method:

```cpp
void VulkanExample::initWindow()
{

}
```

If you're targeting Windows as well, you should surround that by the `#if defined(__linux__)` directives.

# Our Variables

We'll need to keep track of a few variables in our class. I'll be adding these:

```cpp
xcb_connection_t * connection;
xcb_window_t window;
xcb_screen_t * screen;
xcb_atom_t wmProtocols;
xcb_atom_t wmDeleteWin;
```

to our `VulkanExample` class.

# Getting a Display

XCB will allow us to interact with the X server, but we'll need to get a display first. We can use the `xcb_connect` method to get the value of the `DISPLAY` environment variable. Here's the definition:

```cpp
xcb_connection_t* xcb_connect(
  const char * displayname,
  int * screenp);
```

Like I said, we're going to be getting the value of the `DISPLAY` variable. Because we don't know it already, we should pass in `NULL` for that argument. Valid usage for this method looks like this:

```cpp
int screenp = 0;
connection = xcb_connect(NULL, &screenp);
```

Once we've called `xcb_connect`, we should check for any connection errors. We can use the `xcb_connection_has_error` method for this. The definition is simply:

```cpp
int xcb_connection_has_error(xcb_connection_t * c);
```

Valid usage would therefore look like this:

```cpp
if (xcb_connection_has_error(connection))
  exitOnError("Failed to connect to X server using XCB.");
```

Now that we've handled any errors, we need to get available screens. In order to get data from the X server, we can use `xcb_get_setup`. Then we can get a screen iterator from the setup using `xcb_setup_roots_iterator`. Once we've done that, we should loop through the screens using `xcb_screen_next`. The defintions looks like this:

```cpp
// xcb_get_setup - Access the data returned by the server
const struct xcb_setup_t* xcb_get_setup(xcb_connection_t * c);
// xcb_setup_roots_iterator - Get an iterator for screens
xcb_screen_iterator_t xcb_setup_roots_iterator(const xcb_setup_t * R);
// xcb_screen_next - Get the next element of the xcb_screen_iterator_t
void xcb_screen_next(xcb_screen_iterator_t * i);
```

Together the code looks like this:

```cpp
xcb_screen_iterator_t iter =
    xcb_setup_roots_iterator(xcb_get_setup(connection));

for (int s = screenp; s > 0; s--)
    xcb_screen_next(&iter);
```

We can then say:

```cpp
screen = iter.data;
```

# Creating a Window

TODO

# Close Button

TODO

# Update Loop

TODO
