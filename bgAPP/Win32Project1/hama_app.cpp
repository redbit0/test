/******************************************************************************
 * hama_app.cpp
 ******************************************************************************
 * 
 ******************************************************************************
 * All rights reserved by somma (fixbrain@gmail.com)
 ******************************************************************************
 * 2013/10/15   22:14 created
******************************************************************************/
#include "stdafx.h"
#include "scm_context.h"


static pscm_context _scm_context = NULL;

static const wchar_t* _hama_driver_name = L"hama.sys";
static const wchar_t* _hama_service_name = L"hama";
static const wchar_t* _hama_service_display = L"BoBoB";

enum hama_command 
{
	hc_exit			= 0,
	hc_start		= 1,
	hc_stop			= 2, 

	hc_max			= 3
};

bool show_command(_Out_ hama_command& command);
bool process_command(_In_ hama_command command);


__int64 ret64()
{
	return 0x10000000ffffffff;
}

/**
 * @brief	
 * @param	
 * @see		
 * @remarks	
 * @code		
 * @endcode	
 * @return	
**/
int _tmain(int argc, _TCHAR* argv[])
{
	UNREFERENCED_PARAMETER(argc);
	UNREFERENCED_PARAMETER(argv);

	log_msg L"===========================================================" log_end
	log_msg L"                  [ hama application ]                     " log_end
	log_msg L"                                                           " log_end
	log_msg L"                              by somma (fixbrain@gmail.com)" log_end
	log_msg L"===========================================================" log_end


	//> command loop
	hama_command cmd = hc_exit;
	for(;;)
	{
		if (true != show_command(cmd))
		{
			continue;
		}

		if (hc_exit == cmd)
		{
			log_info L"press any key to terminate..." log_end			
			break;
		}

		if (true != process_command(cmd))
		{
			log_err L"process_command(cmd=%u) failed" log_end
			continue;
		}
	}

	if (NULL != _scm_context) 
	{
		delete _scm_context; 
		_scm_context = NULL;
	}
	
	return 0;
}

/**
 * @brief	
 * @param	
 * @see		
 * @remarks	
 * @code		
 * @endcode	
 * @return	
**/
bool show_command(_Out_ hama_command& command)
{
	log_msg L"\ncommands available..." log_end
	log_msg L"___________________________________________________________" log_end
	log_msg L"    0 - terminate" log_end
	log_msg L"    1 - start " log_end
	log_msg L"    2 - stop  " log_end
	std::wcout << L">> ";

	UINT32 temp = 0;
	std::wstring str_cmd;
	std::wcin >> str_cmd;

	std::wstringstream convert;
	convert << str_cmd;
	convert >> temp;
	
	//> check invalid input 
	if(true == convert.fail() || (temp < hc_exit || temp > hc_max))
	{ 
		log_err L"invalid command = %s", str_cmd.c_str() log_end
		return false; 
	}

	command = (hama_command)temp;
	return true;
}

/**
 * @brief	
 * @param	
 * @see		
 * @remarks	
 * @code		
 * @endcode	
 * @return	
**/
bool process_command(_In_ hama_command command)
{
	bool ret = false;

	switch (command)
	{
	case hc_start:
		{
			_ASSERTE(NULL == _scm_context);
			if (NULL != _scm_context) return true;

			
			std::wstring module_dir;
			if (true != get_current_module_dir(module_dir))
			{
				log_err L"get_current_module_dir()" log_end
				break;
			}

			std::wstring driver_path(module_dir);
			driver_path += L"\\";
			driver_path += _hama_driver_name;

			if (true != is_file_existsW(driver_path.c_str()))
			{
				log_err L"no driver file = %s", driver_path.c_str() log_end
				break;
			}
		
			_scm_context = new scm_context(
								driver_path.c_str(), 
								_hama_service_name, 
								_hama_service_display, 
								true
								); 
			_ASSERTE(NULL != _scm_context);
			if (NULL == _scm_context)
			{
				log_err L"insufficient resources for allocate scm_context()" log_end
				break;
			}

			if (true != _scm_context->install_driver())
			{
				log_err L"scm_context::install_driver()" log_end
				delete _scm_context; _scm_context = NULL;
				break;
			}

			if (true != _scm_context->start_driver())
			{
				log_err L"scm_context::start_driver()" log_end
				//delete _scm_context; _scm_context = NULL;
				break;
			}

			log_info L"%s service started successfully", _hama_service_name log_end
			ret = true;
			break;
		}
	case hc_stop:
		{
			if (NULL == _scm_context) return true;			// just ignore this situation

			_scm_context->start_driver();
			_scm_context->uninstall_driver();
			log_info L"%s service stopped successfully", _hama_service_name log_end
			ret = true;
			break;
		}

	default:
		log_err L"invalid command = %u", command log_end
		break;
	}

	return ret;
}
