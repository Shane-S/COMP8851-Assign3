#include <iostream>
#include <unordered_map>
#include <string>
#include <SDL2pp/SDL2pp.hh>
#include <SDL2/SDL_main.h>
#include <memory>
#include "GameManager.h"
#include "GameScreen.h"
#include "Input.h"

using std::string;
using std::unordered_map;
using std::shared_ptr;

int main(int argc, char** argv) {
	try {
		using namespace SDL2pp;
		uint32_t subsystems = SDL_INIT_VIDEO | SDL_INIT_AUDIO;
		SDL sdl(subsystems);
		SDLTTF sdl_ttf;
		SDLImage image;
		SDLMixer sdlMixer(MIX_INIT_OGG | MIX_INIT_MP3);
        Mixer mixer(44100, MIX_DEFAULT_FORMAT, 2, 2048);

		unordered_map<string, string> soundEffects;

		// Straightforward wrappers around corresponding SDL2 objects
		// These take full care of proper object destruction and error checking
		Window window("Quadtree Rendering", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 1280, 720, SDL_WINDOW_RESIZABLE);//SDL_WINDOW_FULLSCREEN);
		Renderer renderer(window, -1, SDL_RENDERER_ACCELERATED);

		unordered_map<string, shared_ptr<Screen>> screens;
		shared_ptr<Screen> rendering(new GameScreen);

		screens.insert({ "rendering", rendering });

        std::string startScreen("rendering");
		GameManager::_instance = new GameManager(std::move(sdl), std::move(image), std::move(mixer), std::move(sdl_ttf), std::move(window), std::move(renderer), screens);
		GameManager::_instance->Loop(startScreen);
	}
	catch (SDL2pp::Exception& e) {
		// Exception stores SDL_GetError() result and name of function which failed
		std::cerr << "Error in: " << e.GetSDLFunction() << std::endl;
		std::cerr << "  Reason: " << e.GetSDLError() << std::endl;
		std::cin.get();
	}
	catch (std::exception& e) {
		// This also works (e.g. "SDL_Init failed: No available video device")
		std::cerr << e.what() << std::endl;
	}
	return 0;
}