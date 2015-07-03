-- Application global options
acv_options = {
	verbosity = 1,
	script_over_tcp = true,
	script_port = 5555,
	gui = true
}

-- Instance declaration
instances = {
	{
		name = "in",
		plugin = "frame_in",
		options = {
			source = "/home/nguyen/clipboard/LevisHK_Cam6_12012014140000.MP4",
			recover_on_error = true,
			retry_interval = 1,
			read_timeout = 5
		}
	},
	{
		 name = "md",
		 plugin = "noob_md",
		 options = {
			 threshold = 1.0
		 }
	},
	{
		 name = "out",
		 plugin = "frame_out",
		 options = {
			 destination = "/home/nguyen/testout.mp4"
		 }
	}
}

-- Instance wiring
wires = {
	{ "in", "video", "md", "video" },
	{ "in", "video", "out", "vin" }
}

