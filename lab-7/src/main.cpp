#include "VideoProcessor.hpp"

int main() {
    VideoProcessor processor;
    
    if (!processor.initialize(0)) {
        return -1;
    }
    
    processor.run();
    
    return 0;
}
