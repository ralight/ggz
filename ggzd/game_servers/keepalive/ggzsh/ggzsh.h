#ifndef GGZ_SCRIPTING_HOST
#define GGZ_SCRIPTING_HOST

// The GGZ Scripting Host class
class GGZScriptingHost
{
	public:
		// Constructor
		GGZScriptingHost();
		// Destructor
		~GGZScriptingHost();
		// Add an external script resource
		void addScript(const char *scriptname);
		// Call a method from the available scripts
		void executeMethod(const char *methodstring);
		// Output information
		void information();

	private:
		// Available modes for each script or method
		enum ScriptModes
		{
			ModePython,
			ModeRuby
		};
};

#endif

