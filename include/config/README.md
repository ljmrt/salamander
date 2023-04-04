# Salamander Configuration

This(.) is the Salamander configuration directory. Here, you will find Salamander configuration files(file extension ".scfg"). These control various parts of application, as shown by the various configuration files below.

Configuration file tree:
```diff
.
├── logging.scfg # Control over the severity and type of logged messages.
└── window.scfg  # Control over the window name and dimensions.
```

Syntax is database-oriented and relatively easy. The value "key" or name comes first, seperated with a ":", and then the value follows. The character "#" specifies that anything following is a comment.

Below is an example of a Salamander configuration file(file extension ".scfg").
```diff
# LINE COMMENT
KEY:VALUE#EOL COMMENT
# NOTE: SEPERATOR(':') AND EOL INDENTATION ARE ALLOWED
KEY : VALUE  # EOL COMMENT
MAIN_WINDOW_WIDTH : 800  # Application main window width.
```

##
