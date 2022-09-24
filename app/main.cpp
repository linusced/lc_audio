#include "config.hpp"
#include "lc_audio/lc_audio.hpp"

int main(int argc, char const *argv[])
{
    std::cout << PROJECT_NAME << " v." << PROJECT_VERSION << '\n';
    std::cout << lc_audio::frequency<float>(2, 2) << '\n';
    return 0;
}