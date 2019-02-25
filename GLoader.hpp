#include <string>
#include <unordered_map>
#include <exception>

#ifdef _WIN32
#include <windows.h>
#else
#include <dlfcn.h>
#endif

template<typename Data>
struct GLoadedData {
	std::string	libname;
	Data		data;
};

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

		template<typename Data>
		#ifdef _WIN32
		GLoadedData<Data>	load(const std::string &libname, const std::string &name)
		#else
		GLoadedData<Data>	load(const std::string &libname, const std::string &name, const int &flag = RTLD_LAZY)
		#endif
		{
			#ifdef _WIN32
			open_lib(libname);
			Data	data = (Data)GetProcAddress(_hdlrs[libname], name.c_str());

			if (!data) {
				FreeLibrary(_hdlrs[libname]);
				throw std::exception();
			}
			#else
			open_lib(libname, flag);
			Data	data = (Data)dlsym(_hdlrs[libname], name.c_str());

			if (!data || dlerror()) {
				dlclose(_hdlrs[libname]);
				throw std::exception();
			}
			#endif
			return GLoadedData<Data>{libname, data};
		}

		void	unload(const std::string &libname) {
			if (_hdlrs.find(libname) != _hdlrs.end()) {
				#ifdef _WIN32
				FreeLibrary(_hdlrs[libname]);
				#else
				dlclose(_hdlrs[libname]);
				#endif
			}
		}

	private:
		#ifdef _WIN32
		void	open_lib(const std::string &libname)
		#else
		void	open_lib(const std::string &libname, const int &flag)
		#endif
		{
			if (_hdlrs.find(libname) != _hdlrs.end())
				return;
			#ifdef _WIN32
			HMODULE	hdlr;

			hdlr = LoadLibraryA(libname.c_str());
			if (!hdlr)
				throw std::exception();
			#else
			void	*hdlr;

			hdlr = dlopen(libname.c_str(), flag);
			if (!hdlr || dlerror())
				throw std::exception();
			#endif
			_hdlrs[libname] = hdlr;
		}

		/* data */
		#ifdef _WIN32
			std::unordered_map<std::string, HMODULE>	_hdlrs;
		#else
			std::unordered_map<std::string, void *>	_hdlrs;
		#endif
};
