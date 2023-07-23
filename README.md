1. Install Visual Studio Community (C++ environment)  
2. Clone repo  
3. Install and setup wxWidgets 3.X  
	3.1. Download zip containing source code from wxWidgets website  
	3.2. Extract source code to its own folder  
	3.3. Open VS project in <extracted_folder>/build/msw/wx_vc17.sln  
	3.4. Build wxWidgets by going into build->batch_build..., Press Select All and then Build.  
	3.5. Once build finishes, add the system environment variable WXWIN with the value <extracted_folder>  
	3.6. Restart visual studio and open LilyNES solution (important: don't open the  folder, open the solution).  
	3.7. Right click the solution in solution explorer->Properties  
	3.8. Change configuration to All Configurations  
	3.9. Under C/C++->General->Additional Include Directories add "$(WXWIN)\include", "$(WXWIN)\include\msvc"  
	3.10. Under Linker->General->Additional Library Directories add "$(WXWIN)\lib\vc_x64_lib"  
4. Install and setup SDL2:  
	4.1. Download SDL2-devel-2.X-VC.zip from: https://github.com/libsdl-org/SDL/releases  
	4.2. Extract zip content to its own folder  
	4.3. Download SDL2-ttf-devel-2.X-VC.zip from: https://github.com/libsdl-org/SDL_ttf/releases.  
	4.4. Extract the SDL_ttf.h and place it in <sdl_extracted_folder>\include  
	4.5. Extract the SDL2_ttf.lib and SDL2_ttf.dll files and place them in <sdl_extracted_folder>\lib\x64  
	4.6. Add environment variable "SDLDir" with the value being <sdl_extracted_folder>  
	4.7. To the system PATH environment variable, add <stdl_extracted_folder>\lib\x64  
	4.8. Restart visual studio and open LilyNES solution  
	4.9. Right click the solution in solution explorer->Properties  
	4.10. Change configuration to All Configurations  
	4.11. Under VC++ Directories->Include Directories add "$(SDLDir)\include"  
	4.12. Under VC++ Directories->Library Directories add "$(SDLDir)\lib\x64"  
	4.13. Under Linker->Input->Additional Dependencies add "SDL2.lib", "SDL2main.lib", "SDL2_ttf.lib"  
5. Install and setup Boost:  
	5.1. Download zip from Boost website  
	5.2. Extract zip content to its own folder  
	5.3. Add environtment variable "BoostDir" with the value being <boost_extracted_folder>  
	5.4. Run bootstrap.bat in <boost_extracted_folder>, and then run b2  
	5.5. Restart visual studio and open LilyNES solution  
	5.6. Right click the solution in solution explorer->Properties  
	5.7. Change configuration to All Configurations  
	5.8. Under VC++ Directories->Include Directories add "$(BoostDir)"  
	5.9. Under VC++ Directories->Library Directories add "$(BoostDir)/stage/lib"  
6. Build LilyNES in release mode  