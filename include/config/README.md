# Salamander Configuration

This(.) is the Salamander configuration directory. Here, you will find Salamander configuration files(file extension ".scfg"). These control various parts of application, as shown by the various configuration files below.

Configuration file tree:
```diff
.
├── logging.scfg   # Logged message type and severity configuration.
├── renderer.scfg  # Max frames in flight configuration.
└── window.scfg    # Window name and default dimensions configuration.
```

Syntax is database-oriented and relatively easy. The "key" or name comes first, seperated with a ":", and then the value follows. The character "#" specifies that anything following is a comment.

Below is an example of a Salamander configuration file(file extension ".scfg").
```diff
# Line comment

KEY:VALUE#EOL comment

# Seperator(':') or EOL comments can be indented or spaced out.
KEY : VALUE  # EOL comment

# Example from "window.scfg".
MAIN_WINDOW_WIDTH : 800  # The application's default window width.
```
