#include <string>
#include <unordered_map>
#include <exception>

#ifdef _WIN32
#include <windows.h>
#else
#include <dlfcn.h>
#endif

class GLoader {
	public:
		~GLoader() {
			for (auto &&hdlr : _hdlrs) {
				#ifdef _WIN32
				FreeLibrary(hdlr.second);
				#else
				dlclose(hdlr.second);
				#endif
			}
		}

		template<typename Symbole>
		#ifdef _WIN32
		Symbole	load(const std::string &lib_path, const std::string &symbole_name)
		#else
		Symbole	load(const std::string &lib_path, const std::string &symbole_name, const int &flag = RTLD_LAZY)
		#endif
		{
			#ifdef _WIN32
			open_lib(lib_path);
			Symbole	sym = (Symbole)GetProcAddress(_hdlrs[lib_path], symbole_name.c_str());

			if (!sym) {
				FreeLibrary(_hdlrs[lib_path]);
				throw std::exception();
			}
			#else
			open_lib(lib_path, flag);
			Symbole	sym = (Data)dlsym(_hdlrs[lib_path], symbole_name.c_str());

			if (!sym || dlerror()) {
				dlclose(_hdlrs[lib_path]);
				throw std::exception();
			}
			#endif
			return sym;
		}

		void	unload(const std::string &lib_path) {
			if (_hdlrs.find(lib_path) != _hdlrs.end()) {
				#ifdef _WIN32
				FreeLibrary(_hdlrs[lib_path]);
				#else
				dlclose(_hdlrs[lib_path]);
				#endif
			}
		}

	private:
		#ifdef _WIN32
		void	open_lib(const std::string &lib_path)
		#else
		void	open_lib(const std::string &lib_path, const int &flag)
		#endif
		{
			if (_hdlrs.find(lib_path) != _hdlrs.end())
				return;
			#ifdef _WIN32
			HMODULE	hdlr;

			hdlr = LoadLibraryA(lib_path.c_str());
			if (!hdlr)
				throw std::exception();
			#else
			void	*hdlr;

			hdlr = dlopen(lib_path.c_str(), flag);
			if (!hdlr || dlerror())
				throw std::exception();
			#endif
			_hdlrs[lib_path] = hdlr;
		}

		/* data */
		#ifdef _WIN32
			std::unordered_map<std::string, HMODULE>	_hdlrs;
		#else
			std::unordered_map<std::string, void *>	_hdlrs;
		#endif
};
