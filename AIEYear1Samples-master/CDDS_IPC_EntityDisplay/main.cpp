/*******************************************************************************************
*
*   raylib [core] example - Basic window
*
*   Welcome to raylib!
*
*   To test examples, just press F6 and execute raylib_compile_execute script
*   Note that compiled executable is placed in the same folder as .c file
*
*   You can find all basic examples on C:\raylib\raylib\examples folder or
*   raylib official webpage: www.raylib.com
*
*   Enjoy using raylib. :)
*
*   This example has been created using raylib 1.0 (www.raylib.com)
*   raylib is licensed under an unmodified zlib/libpng license (View raylib.h for details)
*
*   Copyright (c) 2014 Ramon Santamaria (@raysan5)
*
********************************************************************************************/

#include "raylib.h"
#include "EntityDisplayApp.h"
#include <iostream>

/*
TUTORIAL:
One project (CDDS_IPC_EntityEditor) contains an array of Entity structs which contains properties such
as position, size, rotation, speed and colour. During the Application�s update() method it uses the
RayGUI library to allow us to edit the values within the structs. It also updates the position based on the
speed and rotation of the entities, and during the Application�s draw() method it displays the Entity
structs as simple coloured boxes.

The second project (CDDS_IPC_EntityDisplay) contains a std::vector of Entity structs that starts empty. It
also attempts to draw the contents of the std::vector, except that nothing appears because it is empty.

You are tasked with sharing the contents of the Entity array from project 1 with project 2 so that project
2 can also display the Entity structs correctly. You will need to send project 2 the number of entities it
needs to display. Do not hard code this! If you need to change project 2 from using a std::vector of
Entity structs then you can, but the applications should accurately display the same entities using the
correct positions and colours.

ZORA:
1: Because the contents of project 1 are to be shared withproject 2, project 1 will be the creator of the named shared memory and project 2 will be the opener.
2: Project 2 will need to receive the number of entities it has to display, but this is not known at the time that it opens the HANDLE. I should be able to dynamically calculate this with the size of the array divided by the size of the element at position 0.
3: 
*/

int main(int argc, char* argv[])
{
    float deltaTime = 0;
    EntityDisplayApp app(800, 450);

    // Initialization
    //--------------------------------------------------------------------------------------
    app.Startup();    
    //--------------------------------------------------------------------------------------

    // NAMED SHARED MEMORY SETUP START vvvvv
    // +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    /*
    ZORA: OpenFileMapping is the corresponding function to CreateFileMapping (from the creating application, and a part of the windows.h library). This function also returns a HANDLE, or a nullptr in the event of an error.

    The OpenFileMapping function DOES NOT know in advance the size of the shared memory which it is going to be accessing.
    */

    HANDLE fileHandle = OpenFileMapping(
        FILE_MAP_ALL_ACCESS,            // ZORA: The access level we want this application to have, which could be full access, read only, write only, etc.
        FALSE,                          // ZORA: Determines whether or not processes created within this one will also be permitted to access the named shared memory. I'm not sure if this analogy is precise, but this seems to equate approximately to a protected/private access level.
        L"EntitySharedMemory");         // ZORA: The name of the shared memory we wish to access. This must match the name from the creating application exactly.

    // ZORA: Where the creation of the file map fails, perform a debug printout
    if (fileHandle == nullptr) {
#ifndef NDEBUG
        std::cout << "Could not create file mapping object (application 2): " << GetLastError() << std::endl;
#endif
        return 1;
    }

    /* ZORA: The memory allocated by CreateFileMapping is hidden from all applications within a virtual file system. Each application requires a temporary pointer to that virtual file in order to access it.The MapViewOfFile function creates a void pointer so that we can refer to any object type, but we want the same type as the object at the shared memory location.
    */
    // ZORA: 1) Determine the number of items in the array according to data shared by the first file.
    DWORD arraySize = 0;

    unsigned int* size = (unsigned int*)MapViewOfFile(
        fileHandle,             // ZORA: Target HANDLE
        FILE_MAP_ALL_ACCESS,    // ZORA: Type of access, per CreateFileMapping
        0,                      // ZORA: Offset within the memory allocation of the named shared memory for dynamic or selective access to a specific area in the target memory.
        0,                      // ZORA: Offset within the memory allocation of the named shared memory for dynamic or selective access to a specific area in the target memory.
        sizeof(unsigned int));  // ZORA: The size of the named shared memory to map

    // Assign the memory 
    arraySize = *size;

    // ZORA: Where the creation of the pointer to view the file map fails, perform a debug printout
    if (size == nullptr) {
#ifndef NDEBUG
        std::cout << "Could not map view of file (for the size): " << GetLastError() << std::endl;
#endif
        // ZORA: This is for identical, but even more important, reasons as file I/O closures
        CloseHandle(fileHandle);
        return 1;
    }


    Entity* data = (Entity*)MapViewOfFile(
        fileHandle,                     // ZORA: Our target HANDLE
        FILE_MAP_ALL_ACCESS,            // ZORA: The type of access, per CreateFileMapping and OpenFileMapping.
        0,                              // ZORA: Offset within the memory allocation of the named shared memory for dynamic or selective access to a specific area in the target memory.
        sizeof(unsigned int),           // ZORA: Offset within the memory allocation of the named shared memory for dynamic or selective access to a specific area in the target memory.
        sizeof(Entity) * arraySize);    // ZORA: The size of the named shared memory 

    // ZORA: Where the creation of the pointer to view the file map fails, perform a debug printout
    if (data == nullptr) {
#ifndef NDEBUG
        std::cout << "Could not map view of file (for the array): " << GetLastError() << std::endl;
#endif
        // ZORA: This is for identical, but even more important, reasons as file I/O closures
        CloseHandle(fileHandle);
        return 1;
    }

    // Populate the array in this application with each of the elements inside the array of the shared memory.
    for (int i = 0; i < arraySize; i++) {
        app.GetArray().push_back(data[i]);
    }

#ifndef NDEBUG
    std::cout << "Array transferred successfully." << std::endl;
#endif

    // +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ 
    // NAMED SHARED MEMORY SETUP FINISH ^^^^^



    // Main game loop
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        deltaTime = GetFrameTime();

        // Update
        //----------------------------------------------------------------------------------
        app.Update(deltaTime);
        //----------------------------------------------------------------------------------

        // Draw
        //----------------------------------------------------------------------------------
        app.Draw();
        //----------------------------------------------------------------------------------
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------   
    app.Shutdown();
    //--------------------------------------------------------------------------------------

    // De-initialisation of the named shared memory
    // +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

    // ZORA: Similar to closing a file, we must close the 'mapping' of an allocation of named shared memory. From the tute: "Unmapping the pointer doesn�t delete named shared memory, it simply invalidates the pointer�s access to the memory."
    UnmapViewOfFile(size);
    UnmapViewOfFile(data);

    // ZORA: This is for identical, but even more important, reasons as file I/O closures
    CloseHandle(fileHandle);

    // +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

    return 0;
}