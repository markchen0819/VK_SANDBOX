#pragma once
namespace IHCEngine::Graphics
{
    struct FluidParticle
    {
        glm::vec4 position;
        glm::vec4 predictPosition; // important to make system stable
        glm::vec4 velocity;
        glm::vec4 color;
        float pressure = 0.0f;
        float density = 0.0f;
    };

    struct SpacialEntry
    {
	    glm::uint index = 999; // particle ID , 0 ~ particle Count
        glm::uint hash = 999; // cellID, created using particle's position 
	    glm::uint key = 999;   // key to cell
        glm::uint dummy = 999;
    };

    struct StartIndexForSpacialEntry
    {
        glm::uint index= 999; 
    };

    // After bitonic merge sort, SpacialEntries are sorted by key
	// therefore adjacent entries are in the same space.
	// ex: 1 1 1 3 3 5 8 8 8
	//
	// we create an index mapping array for fast access
    // cell key < - > first entry
    // Array Index :      0  1  2  3  4  5  6  7
    // Hash Key Values:   1  2  3  4  5  6  7  8
	// Values :          [-, -, 0, -, 3, 5, -, 6]
    //
    // We want to know
    // what points are in this cell?
    // cell -> calculate hash key to be 8 -> look up at array index 7
	// -> shown that the first entry is at position 6, our target space is key 8
    // -> we can iterate start from position 6 to when the key isn't 8
	// If cell calculate key to be 7, there is no entry, no particles in this spatial cell
}