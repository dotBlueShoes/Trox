#include "Framework.hpp"
#include "Windows/PreviewWindow/PreviewWindow.hpp"
#include "Windows/DialogWindows/WindowAbout.hpp"
#include "Windows/WindowEditor.hpp"
#include "Windows/WindowRight.hpp"
#include "Windows/WindowLeft.hpp"

namespace windows::mainWindow {
	
	
	const Editor editor { 10, 10, 80, 20 };
	const auto SC_DRAG = SC_SIZE + 9;
	//winapi::rect previousWindowPosition;
	
	namespace event {

		inline proceeded Create(const winapi::windowHandle& mainWindow) {
			if constexpr (DEBUG) debug::LogInfo("(CALL) Window-Main:Event-Create");
			if constexpr (SYSTEM_VERSION == SystemVersion::Windows10) 
				if (darkmode::isSupported) {
					/// Refresh titlebar theme color.
					darkmode::AllowDarkModeForWindow(mainWindow);
					darkmode::RefreshTitleBarTheme(mainWindow);
				}
				
			//MessageBoxExW(mainWindow, L"Main", L"Call", MB_YESNO, 0);
				/// Creation of inner windows that have their own drawing system. 
			windows::CreateEditor(mainProcess, mainWindow, editor);
			
			HWND button = CreateWindowW( 
				L"BUTTON",  // Predefined class; Unicode assumed 
				L"Ok",      // Button text 
				WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,  // Styles 
				10,         // x position 
				40,         // y position 
				80,        // Button width
				20,        // Button height
				mainWindow,     // Parent window
				NULL,       // No menu.
				mainProcess,
				NULL
			);      // Pointer not needed.
			
				//registry::AddRegistryKey(window);
				//registry::RemoveRegistryKey(window);
				//windows::CreatePreviewWindow(mainProcess, mainWindow);
				// Making window in window.... It's hard
				// https://stackoverflow.com/questions/42734429/win32-c-child-window-is-not-visible
				// 2 windowClass windowProperties;
				// 2 windowProperties.cbSize = ( sizeof(windowClass) );
				// 2 
				// 2 windowProperties.style			= NULL; //
				// 2 windowProperties.lpfnWndProc	= (windowProcedure)WindowOtherProcedure;
				// 2 windowProperties.cbClsExtra		= 0;
				// 2 windowProperties.cbWndExtra		= 0;
				// 2 windowProperties.hInstance		= mainProcess;
				// 2 windowProperties.hIcon			= nullptr;
				// 2 windowProperties.hCursor		= nullptr;
				// 2 windowProperties.hbrBackground	= themes::backgroundSecondary.Get();
				// 2 windowProperties.lpszMenuName	= nullptr;
				// 2 windowProperties.lpszClassName	= L"WindowClass2Name";
				// 2 windowProperties.hIconSm		= nullptr;
				// 2 
				// 2 RegisterClassExW(&windowProperties);
				// 2 
				// 2 if (!CreateWindowExW(0, L"WindowClass2Name", L"Window2Name", WS_CHILD | WS_SIZEBOX | WS_CLIPSIBLINGS | WS_OVERLAPPEDWINDOW , 0, 0, 400, 300, window, nullptr, mainProcess, NULL))
				// 2 	MessageBoxExW(window, L"Here", L"Oh yeah", MB_YESNO, 0);
				// 2 
				// 2 ShowWindow(window, SW_SHOW);
				// 2 UpdateWindow(window);
				
			return proceeded::True;
		}
	
		inline proceeded Destroy() {
			if constexpr (DEBUG) debug::LogInfo("(CALL) Window-Main:Event-Destory");
			brushes::Destroy();
			PostQuitMessage(0); /// Call to the thread queue itself that we're finished.
			return proceeded::True;
		}
	
		inline proceeded Paint(const winapi::windowHandle& window) {
			if constexpr (DEBUG) debug::LogInfo("(CALL) Window-Main:Event-Paint");
			const array<winapi::wchar, 6> label1 { L"Texta" };
			const array<winapi::wchar, 6> label2 { L"Textb" };
			
			winapi::windowDrawContext drawContext;
			winapi::rect clientArea { 0 };
			
			{
				winapi::displayContextHandle displayContext ( BeginPaint(window, &drawContext) );
				
				/// To get the whole "background" area.
				GetClientRect(window, &clientArea);
				
				/// Setting up the background and text color. This takes action in windows created here.
				SetBkColor(displayContext, brushes::colors->primar);
				SetTextColor(displayContext, brushes::colors->text);
				
				/// If we woudn't recreate some window/s with each draw we could use this
				/// to simply make our background the default we wanted. SetBkColor wouldn't be needed then.
				//FillRect(displayContext, &clientArea, brushes::primar.Get());
				
				/// For WindowEditor
				windows::PaintBorder(displayContext, editor, 1);
				
				/// !
				/// When we're drawing text using ExtTextOutW
				///  we're using areas to draw at.
				///  coordinates does not relate to textArea in any way!!!
				/// it can be a nullptr. However if we do specify it and one
				/// is on top of the other then the new one will be applied on the old one
				/// ! ALSO those areas are corners positions so if one set to 400 other to 600 then we have 200 in size.
				winapi::rect text1Area { 0, 0, 300, 300 };
				winapi::rect text2Area { 300, 300, 600, 600 };
				
				/// Displing some text in a clientArea making the background color change.
				ExtTextOutW (
					displayContext,			/// on what we are drawing
					100,						/// x coordinate
					0,						/// y coordinate
					ETO_OPAQUE,				/// styles
					&text1Area,			/// rect specyfing the window coordinates
					label1.Pointer(),		/// string
					label1.Length(),		/// string length
					nullptr					/// distance between letters
				);
				
				ExtTextOutW (
					displayContext,			/// on what we are drawing
					300,						/// x coordinate
					0,						/// y coordinate
					ETO_OPAQUE,				/// styles
					&text2Area,			/// rect specyfing the window coordinates
					label2.Pointer(),		/// string
					label2.Length(),		/// string length
					nullptr					/// distance between letters
				);
				
				EndPaint(window, &drawContext);
			}
	
			return proceeded::True;
		}
	
		inline proceeded MessageAbout(const winapi::windowHandle& window) {
			if constexpr (DEBUG) debug::LogInfo("(CALL) Window-Main:Event-MessageAbout");
			DialogBox(mainProcess, MAKEINTRESOURCE(resourceHandler::windowAboutId), window, (DLGPROC)windows::About);
			return proceeded::True;
		}
		
		inline proceeded SettingChange(
			const winapi::windowHandle& window, 
			const winapi::messageW& wArgument, 
			const winapi::messageL& lArgument
		) {
			if constexpr (DEBUG) debug::LogInfo("(CALL) Window-Main:Event-SettingChange");
			if (darkmode::isSupported)
				if (wArgument == 0)
					return darkmode::CheckMainWindowWhetherImmersiveColorSet(window, (winapi::wchar*)lArgument, messageCounter);
			return proceeded::False;
		}
	
		inline proceeded ThemeChange(winapi::windowHandle window) {
			if constexpr (DEBUG) debug::LogInfo("(CALL) Window-Main:Event-ThemeChange");
			
			darkmode::RefreshTitleBarTheme(window);
		
			if (winapi::window::event::uahmenubar::menuTheme) {
				CloseThemeData(winapi::window::event::uahmenubar::menuTheme);
				winapi::window::event::uahmenubar::menuTheme = nullptr;
			}
		
			if (darkmode::isEnabled) brushes::ChangePalette(theme::dark);
			else brushes::ChangePalette(theme::light);
		
			/// This makes brushes white as the data holded there is no longer available.
			brushes::Destroy(); 
			brushes::Initialize();
			InvalidateRect(window, NULL, TRUE);
			DrawMenuBar(window);
		
			return proceeded::True;
		}
	
		inline proceeded Resize(
			winapi::windowHandle window, 
			uint32 state, 
			int clientX, 
			int clientY
		) {
			if constexpr (DEBUG) debug::LogInfo("(CALL) Window-Main:Event-Resize");
			return proceeded::False;
		}
	
		inline proceeded Default(
			winapi::windowHandle window, 
			uint32 message, 
			winapi::messageW wArgument, 
			winapi::messageL lArgument
		) {
			return (proceeded)winapi::DefaultWindowProcedure(window, message, wArgument, lArgument);
		}
	}
	
	proceeded stdcall WindowMainProcedure(
		winapi::windowHandle window,
		input message,
		winapi::messageW wArgument,
		winapi::messageL lArgument
	) {
		namespace menu = mst::winapi::window::event::uahmenubar;
		switch (message) {
	
			case input::Command: {
				switch (winapi::GetMenuInput(wArgument)) {
					case resourceHandler::mainMenuInput::About: return event::MessageAbout(window);
					case resourceHandler::mainMenuInput::Quit: DestroyWindow(window); return proceeded::True;
	
					case resourceHandler::mainMenuInput::MaxMin: {
						
						GetWindowPlacement(window, &windowMode::windowedPlacement);
						if (windowMode::windowedPlacement.showCmd == SW_MAXIMIZE) ShowWindow(window, SW_SHOWDEFAULT);
						else ShowWindow(window, SW_MAXIMIZE);
							
						return proceeded::True;
					} // F11
	
					default: return (proceeded)event::Default(window, (uint32)message, wArgument, lArgument);
				} break;
			}
			
			case input::OnMouseLeftClickDown: {
				//GetWindowRect(window, &previousWindowPosition);
				/// To make the window dragable. 
				PostMessage(window, WM_SYSCOMMAND, SC_DRAG, (LPARAM)NULL);
				return proceeded::False;
			}
	
			/// Clang questions enum values specified outside the used type... 
			///  As i want menu theming to be optional and not any way dirived it stays this way.
			#pragma GCC diagnostic push
			#pragma GCC diagnostic ignored "-Wswitch"
			
			case (input)menu::Event::DrawItem: {
				auto menuItem ( *((menu::UAHDRAWMENUITEM*)lArgument) );
				return menu::DrawMenuItem(
					window, menuItem, 
					brushes::second, 
					brushes::selected, 
					brushes::hovered, 
					(*(brushes::colors)).text
				);
			}
			
			case (input)menu::Event::Draw: {
				auto menuInstance ( *((menu::UAHMENU*)lArgument) );
				return menu::DrawMenu(window, menuInstance, brushes::second);
			}
			
			#pragma GCC diagnostic pop
	
			case input::SettingChange:
				return event::SettingChange(window, wArgument, lArgument);
			
			case input::EraseBackgroundOnCalledInvalidPortion:
				return proceeded::False;
			
			case input::ThemeChange:
				return event::ThemeChange(window);
	
			case input::Create:
				return event::Create(window);
			
			case input::Paint:
				return event::Paint(window);
	
			case input::Destroy:
				return event::Destroy();
			
			case input::NonClientAreaPaint:
			case input::NonClientAreaFocus: {
				winapi::displayContextHandle drawContext { GetWindowDC(window) };
				event::Default(window, (uint32)message, wArgument, lArgument); 		/// We need to get throuh some other paints first.
				menu::DrawBottomLine(window, drawContext, brushes::border.Get());
				return proceeded::True;
			}
			
			case input::Resize:
				event::Resize(window, wArgument, LOWORD(lArgument), HIWORD(lArgument));
				
			default:
				return event::Default(window, (uint32)message, wArgument, lArgument);
			
			/* Keeping it just to get u know thats the option. Change Height/Width
			//case (proceeded)uahmenubar::UAHMenuEvent::MeasureItem:
			//	event::Default(window, message, wArgument, lArgument); // Allow the default window procedure to handle the message. So we have something and we do actions on top of that.
			//	{ using namespace uahmenubar;
			//		uint32 height = (uint32)((*(UAHMEASUREMENUITEM*)lArgument).mis.itemHeight * 4.0 / 3.0),
			//			width = (uint32)((*(UAHMEASUREMENUITEM*)lArgument).mis.itemWidth * 4.0 / 3.0);
			//		MeasureMenuItem((*(UAHMEASUREMENUITEM*)lArgument), height, width);
			//	} return proceeded::True;
			*/
		}
		
		return proceeded::False;
	}
	
}

/* Old Event::Create
inline proceeded Create(const winapi::windowHandle& mainWindow) {
	if constexpr (DEBUG) debug::LogInfo("(CALL) Window-Main:Event-Create");
	if constexpr (SYSTEM_VERSION == SystemVersion::Windows10) 
		if (darkmode::isSupported) {
			/// Refresh titlebar theme color.
			darkmode::AllowDarkModeForWindow(mainWindow);
			darkmode::RefreshTitleBarTheme(mainWindow);
		}

	//MessageBoxEx(window, L"Main", L"Call", MB_YESNO, 0);
	/// Creation of inner windows that have their own drawing system. 
	//windows::CreateEditor(mainProcess, window);
	//registry::AddRegistryKey(window);
	//registry::RemoveRegistryKey(window);
	//windows::CreatePreviewWindow(mainProcess, mainWindow);
	// Making window in window.... It's hard
	// https://stackoverflow.com/questions/42734429/win32-c-child-window-is-not-visible
	// 2 windowClass windowProperties;
	// 2 windowProperties.cbSize = ( sizeof(windowClass) );
	// 2 
	// 2 windowProperties.style			= NULL; //
	// 2 windowProperties.lpfnWndProc	= (windowProcedure)WindowOtherProcedure;
	// 2 windowProperties.cbClsExtra		= 0;
	// 2 windowProperties.cbWndExtra		= 0;
	// 2 windowProperties.hInstance		= mainProcess;
	// 2 windowProperties.hIcon			= nullptr;
	// 2 windowProperties.hCursor		= nullptr;
	// 2 windowProperties.hbrBackground	= themes::backgroundSecondary.Get();
	// 2 windowProperties.lpszMenuName	= nullptr;
	// 2 windowProperties.lpszClassName	= L"WindowClass2Name";
	// 2 windowProperties.hIconSm		= nullptr;
	// 2 
	// 2 RegisterClassExW(&windowProperties);
	// 2 
	// 2 if (!CreateWindowExW(0, L"WindowClass2Name", L"Window2Name", WS_CHILD | WS_SIZEBOX | WS_CLIPSIBLINGS | WS_OVERLAPPEDWINDOW , 0, 0, 400, 300, window, nullptr, mainProcess, NULL))
	// 2 	MessageBoxExW(window, L"Here", L"Oh yeah", MB_YESNO, 0);
	// 2 
	// 2 ShowWindow(window, SW_SHOW);
	// 2 UpdateWindow(window);

	return proceeded::True;
}
*/