RDS_PROTOCOL = {
		 MAX_MESSAGE_LENGTH=65535,

		 MAX_PAYLOAD_LENGTH=65532,

		 VERSION=tonumber("0x05",16),

		 LOGIN_REQUEST=tonumber("0x10",16),

		 LOGIN_SUCCESS=tonumber("0x11",16),

		 LOGIN_FAILURE=tonumber("0x12",16),

		 LOGIN_REDIRECT=tonumber("0x13",16),

		 RECONNECT_REQUEST=tonumber("0x20",16),

		 RECONNECT_SUCCESS=tonumber("0x21",16),

		 RECONNECT_FAILURE=tonumber("0x22",16),

		 SESSION_MESSAGE=tonumber("0x30",16),

		 LOGOUT_REQUEST=tonumber("0x40",16),

		 LOGOUT_SUCCESS=tonumber("0x41",16),

		 CHANNEL_JOIN=tonumber("0x50",16),

		 CHANNEL_LEAVE=tonumber("0x51",16),

		 CHANNEL_MESSAGE=tonumber("0x52",16)
}
