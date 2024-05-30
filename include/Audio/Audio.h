#include <string>

namespace Audio
{
    using AudioId = size_t;

    /**
     * Represents an audio file.
     *
     * Supported audio formats are:
     *  - WAV
     *  - MP3
     */
    class Audio
    {
    public:
        /**
         * Creates a new audio instance.
         *
         * @param path The path to the audio file.
         */
        Audio(std::string path);

        /**
         * Destroys the audio instance.
         */
        ~Audio();

        /**
         * Loads the audio from the given file.
         *
         * @param path The path to the audio file.
         */
        void fromFile(std::string path);

    private:
        static AudioId nextId;

        const AudioId id = nextId++;
    };
}