#include "Framework.hpp"

#include "Windows/WindowMain.hpp"	
#include "MP3/mp3.hpp"

/// ERRORs
/// - ctrl clicking breaks.... the whole app

/// compile and run process.. 
/// 1. compile resource
/// 2. build
/// 3. add manifest (so the buttons appear right..)
/// 4. run


/// TODO:
/// - get rid of top border and in other windows borders that are all around..
/// - windows 10 blur background effect
/// - get onButtonClick logic
/// - keep the style https://i.redd.it/ec2syrbyzl0a1.png
/// - console integration

/// Links
/// - https://devblogs.microsoft.com/oldnewthing/20130909-00/?p=3293 ( OLD NEW THING [SafePointers] )
/// - https://stackoverflow.com/questions/8629718/how-do-i-use-ipropertystore-to-access-mp3-metadata-in-windows-with-c ( It can look easier... kind of )
/// - https://learn.microsoft.com/en-us/windows/win32/properties/props-system-music-displayartist ( All things can be little weird )

/// Keys
/// PKEY_Music_AlbumArtist - System.Music.AlbumArtist
/// PKEY_Music_AlbumArtistSortOverride - System.Music.AlbumArtistSortOverride
/// PKEY_Music_AlbumID - System.Music.AlbumID (This property differentiates albums with the same title from different artists. It is the concatenation of System.Music.AlbumArtist and System.Music.AlbumTitle.)
/// PKEY_Music_AlbumTitle - System.Music.AlbumTitle
/// PKEY_Music_AlbumTitleSortOverride - System.Music.AlbumTitleSortOverride
/// PKEY_Music_Artist - System.Music.Artist
/// PKEY_Music_ArtistSortOverride - System.Music.ArtistSortOverride
/// PKEY_Music_BeatsPerMinute - System.Music.BeatsPerMinute
/// PKEY_Music_Composer - System.Music.Composer
/// PKEY_Music_ComposerSortOverride - System.Music.ComposerSortOverride
/// PKEY_Music_Conductor - System.Music.Conductor
/// PKEY_Music_ContentGroupDescription - System.Music.ContentGroupDescription
/// PKEY_Music_ContentGroupDescription - System.Music.ContentGroupDescription
/// PKEY_Music_DiscNumber - System.Music.ContentGroupDescription
/// PKEY_Music_DisplayArtist - System.Music.DisplayArtist (This property returns the best representation of the album artist for a specific music file based upon System.Music.AlbumArtist, System.Music.Artist, and System.Music.IsCompilation information.)
/// PKEY_Music_Genre - System.Music.Genre
/// PKEY_Music_InitialKey - System.Music.Genre
/// PKEY_Music_IsCompilation - System.Music.IsCompilation
/// PKEY_Music_Lyrics - System.Music.Lyrics
/// PKEY_Music_Mood - System.Music.Mood
/// PKEY_Music_PartOfSet - System.Music.PartOfSet
/// PKEY_Music_Period - System.Music.Period
/// PKEY_Music_SynchronizedLyrics - System.Music.SynchronizedLyrics
/// PKEY_Music_TrackNumber - System.Music.TrackNumber

int32 stdcall wWinMain(
	[in]	winapi::handleInstance process,	/// The process we're given to run our program.
	[out]	winapi::handleInstance ignored,	/// Now has no meaing it's 0 always.
	[in]	winapi::wchar* cmdlineArgs,		/// Contains command line arguments as a unicode string.
	[in]	int32 windowState				/// Flag that says whether the window should appear minimized, maximied, shown normally.
){
	using namespace winapi;
	const vector2<uint64> windowSize ( 1400, 800 );
	
	mainProcess = process; 					/// Setting up the Global
	
	application::Initialize();				/// Initializing Modules, DarkMode.
	resourceHandler::Initialize(process);	/// Loading Strings into arrays.

	if constexpr (SYSTEM_VERSION == SystemVersion::Windows10) 
		brushes::ChangePalette(window::theme::dark);
	
	brushes::Initialize();
	windows::mainWindow::Create(process, brushes::primar.Get(), windowState, windowSize);
	
	{ // MP3
		
		
		/// First, we took the file whose name was passed on the command line 
		///  (fully-qualified path, please) and obtained its property store.
		array<winapi::wchar, 103> sample { L"C:\\Users\\Admin\\Music\\dl\\The Strokes\\The New Abnormal\\1 The Strokes - The Adults Are Talking.mp3" };
		//array<winapi::wchar, 74> sample { L"C:\\Users\\Admin\\Music\\Dire Straits\\On the Night\\01 Calling Elvis.wma" };
		array<winapi::wchar, 15> artistName { L"thestrokesVEVO" };
		
		
		bool found (false);
	
		CoInitialize(NULL);
		CComPtr<IPropertyStore> spps;
		
		if constexpr (DEBUG) {
			HRESULT result = SHGetPropertyStoreFromParsingName (
				sample.Pointer(), 
				nullptr,
				GPS_READWRITE, 
				IID_PPV_ARGS(&spps)
				//__uuidof(IPropertyStore), 
				//(void**)&spps
			);
			if (result != S_OK) mst::debug::LogError("Could not return a property store for an item!");
		} else {
			SHGetPropertyStoreFromParsingName (
				sample.Pointer(), 
				nullptr,
				GPS_READWRITE, 
				IID_PPV_ARGS(&spps)
			);
		}
		
		
		
		/// Get the existing composers
		PWSTR *rgpszComposers;
		ULONG cComposers;
		
		/// Next, we queried the property store for the System.Music.Composer property. 
		///  This property is typed as a multiple-valued string, so we read and write the 
		///  value in the form of a string vector. You could also read and write it as a 
		///  single string: The PropVariantToStringAlloc function represents 
		///  string arrays by joining the strings together, separating them with 
		///  "; " (semicolon and space). However, we access it as an array, because 
		///  that makes it easier to insert and remove individual entries.
		//MP3::GetPropertyAsStringVector (
		//	spps,
		//	PKEY_Music_Composer, /// System.Music.Composer property.
		//	&rgpszComposers,
		//	&cComposers
		//);
		
		//MP3::GetPropertyAsStringVector (
		//	spps,
		//	PKEY_Music_AlbumArtist, /// System.Music.Composer property.
		//	&rgpszComposers,
		//	&cComposers
		//);
		
		MP3::GetPropertyAsStringVector (
			spps,
			PKEY_Music_Artist, /// System.Music.Composer property.
			&rgpszComposers,
			&cComposers
		);
		
		/// Look for "Dvorak, Antonin" and add diacritics
		for (ULONG ulPos = 0; ulPos < cComposers; ulPos++) {
			if (wcscmp(rgpszComposers[ulPos], artistName.Pointer()) == 0) {
				found = true;
				
				/// Swap in the new name
				PWSTR pszOld = rgpszComposers[ulPos];
				rgpszComposers[ulPos] = L"The Strokes";
				
				{ /// Write out the new list of composers
					HRESULT result = MP3::SetPropertyAsStringVector(spps, PKEY_Music_Artist, (PCWSTR *)rgpszComposers, cComposers);
					if (result == S_OK) mst::debug::LogInfo("S_OK!");
				}
				
				/// Swap it back so we can free it
				rgpszComposers[ulPos] = pszOld;
				
				//// Add a little graffiti just because
				//MP3::SetPropertyAsString(spps, PKEY_Comment, L"Kilroy was here");
		
				{
					HRESULT result = spps->Commit();
					if (result == S_OK) mst::debug::LogInfo("S_OK!");
					else if (result == STG_E_ACCESSDENIED) mst::debug::LogInfo("STG_E_ACCESSDENIED!");
					else if (result == E_FAIL) mst::debug::LogInfo("E_FAIL!");
					else mst::debug::LogInfo("Imposible!");
				}
				break;
			}
		}
		
		if (found == false) mst::debug::LogInfo("There is no artist named that way!");
		
		MP3::CoTaskMemFreeArray(rgpszComposers, cComposers);
	}

	{	// Program's main loop.
		retrivedMessage message;
		while (message::Get(message)) {
			if (!accelerator::Translate(message.hwnd, resourceHandler::keys, message)) {
				message::Translate(message);
				message::Dispatch(message);
			}
		} 
	
		application::Destroy();
		return (int32)message.wParam;
	}
	
	return -1;
}