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
#include "EntityEditorApp.h"
#include <iostream>

int main(int argc, char* argv[])
{
    float deltaTime = 0;
    EntityEditorApp app(800, 450);

    // Initialization
    //--------------------------------------------------------------------------------------
    app.Startup();
    //--------------------------------------------------------------------------------------
 
    // NAMED SHARED MEMORY SETUP START vvvvv
    // +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    /* ZORA: CreateFileMapping is the windows.h function to create a virtual file.This method returns a HANDLE object which is like a pointer to a file.If the function fails, the HANDLE will be a nullptr.This is set by the first parameter.Access type is set by the 3rd parameter.

    The size of the memory block to be allocated is recorded in the 4th and 5th parameters, and the Microsoft documentation explains: "If this parameter (5th) and dwMaximumSizeHigh (4th) are 0, the maximum size of the file mapping object is equal to the current size of the file that hFile (the HANDLE) identifies."

    Currently the named shared memory is configured to share memory of a size equal to a user-defined struct called MyData, which is the size of its 5 member variables.

    Memory is allocated at the point when CreateFileMapping is executed so there is no need to use the 'new' keyword to instantiate anything / allocate memory.
        */

    // ZORA: Create a named shared memory file map
    HANDLE fileHandle_01 = CreateFileMapping(
        INVALID_HANDLE_VALUE,	    // a handle to an existing virtual file, or invalid
        nullptr,				    // optional security attributes
        PAGE_READWRITE,			    // read/write access control
        0, sizeof(unsigned int),	// ZORA: The memory needs of the virtual file, determined according to the combination of the size of the array inside the EntityEditorApp instance, plus an unsigned int which will tell the second application, numerically, how many objects to expect in the array
        L"IntSharedMemory");		// ZORA: The string name that the 2nd application will use to access the virtual file
    
    // ZORA: Where the creation of the file map fails, perform a debug printout
    if (fileHandle_01 == nullptr) {
#ifndef NDEBUG
        std::cout << "Could not create file mapping object (application 1): " << GetLastError() << std::endl;
#endif
        return 1;
    }

    else {
#ifndef NDEBUG
        std::cout << "File mapping object created successfully (application 1)." << std::endl;
#endif
    }

    /* ZORA: The memory allocated by CreateFileMapping is hidden from all applications within a virtual file system. Each application requires a temporary pointer to that virtual file in order to access it.The MapViewOfFile function creates a void pointer so that we can refer to any object type, but we want the same type as the object at the shared memory location. The pointer will point to the first element in the array, but the memory allocation size needs to be equal to the size of the entire array.
    */
    // ZORA: Make the volume of objects inside the array known to the other application
    unsigned int* size = (unsigned int*)MapViewOfFile(
        fileHandle_01,          // ZORA: Target HANDLE
        FILE_MAP_ALL_ACCESS,    // ZORA: Type of access, per CreateFileMapping
        0,                      // ZORA: Offset within the memory allocation of the named shared memory for dynamic or selective access to a specific area in the target memory.
        0,                      // ZORA: Offset within the memory allocation of the named shared memory for dynamic or selective access to a specific area in the target memory.
        sizeof(unsigned int));  // ZORA: The size of the named shared memory to map

    *size = app.GetEntityCount();

    // ZORA: Where the creation of the pointer to view the file map fails, perform a debug printout
    if (size == nullptr) {
#ifndef NDEBUG
        std::cout << "Could not map view of file (for the size): " << GetLastError() << std::endl;
#endif
        // ZORA: This is for identical, but even more important, reasons as file I/O closures
        CloseHandle(fileHandle_01);
        return 1;
    }

    else {
#ifndef NDEBUG
        std::cout << "Map viewed successfully (unsigned int)." << std::endl;
#endif
    }

    UnmapViewOfFile(size);
    


    HANDLE fileHandle_02 = CreateFileMapping(
        INVALID_HANDLE_VALUE,	    // a handle to an existing virtual file, or invalid
        nullptr,				    // optional security attributes
        PAGE_READWRITE,			    // read/write access control
        0, size_t(app.GetArraySize()),		// ZORA: The memory needs of the virtual file, determined according to the combination of the size of the array inside the EntityEditorApp instance, plus an unsigned int which will tell the second application, numerically, how many objects to expect in the array
        L"ArraySharedMemory");		// ZORA: The string name that the 2nd application will use to access the virtual file
//
//    // ZORA: Make the array of objects available to the other application
//    Entity* data = (Entity*)MapViewOfFile(
//        fileHandle_02,             // ZORA: Target HANDLE
//        FILE_MAP_ALL_ACCESS,    // ZORA: Type of access, per CreateFileMapping
//        0,                      // ZORA: Offset within the memory allocation of the named shared memory for dynamic or selective access to a specific area in the target memory.
//        0,   // ZORA: Offset within the memory allocation of the named shared memory for dynamic or selective access to a specific area in the target memory.
//        app.GetArraySize());    // ZORA: The size of the named shared memory to map
//
//    // ZORA: Where the creation of the pointer to view the file map fails, perform a debug printout
//    if (data == nullptr) {
//#ifndef NDEBUG
//        std::cout << "Could not map view of file (for the array): " << GetLastError() << std::endl;
//#endif
//        // ZORA: This is for identical, but even more important, reasons as file I/O closures
//        CloseHandle(fileHandle_02);
//        return 1;
//    }
//
//    else {
//#ifndef NDEBUG
//        std::cout << "Map viewed successfully (array)." << std::endl;
//#endif
//    }
//
//    // ZORA: Set the file map pointer so that it points to the memory address of the object at the front of the array of Entities
//    *data = app.ArrayOfEntities();

    
    

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

        // ZORA: Make the array of objects available to the other application
        Entity* data = (Entity*)MapViewOfFile(
            fileHandle_02,          // ZORA: Target HANDLE
            FILE_MAP_ALL_ACCESS,    // ZORA: Type of access, per CreateFileMapping
            0,                      // ZORA: Offset within the memory allocation of the named shared memory for dynamic or selective access to a specific area in the target memory.
            0,                      // ZORA: Offset within the memory allocation of the named shared memory for dynamic or selective access to a specific area in the target memory.
            size_t(app.GetArraySize()));    // ZORA: The size of the named shared memory to map

        // ZORA: Where the creation of the pointer to view the file map fails, perform a debug printout
        if (data == nullptr) {
#ifndef NDEBUG
            std::cout << "Could not map view of file (for the array): " << GetLastError() << std::endl;
#endif
            // ZORA: This is for identical, but even more important, reasons as file I/O closures
            CloseHandle(fileHandle_02);
            return 1;
        }

        else {
#ifndef NDEBUG
            std::cout << "Map viewed successfully (array)." << std::endl;
#endif
        }

        // ZORA: Set the file map pointer so that it points to the memory address of the object at the front of the array of Entities
        app.ArrayOfEntities(data);



        // Draw
        //----------------------------------------------------------------------------------
        app.Draw();
        //----------------------------------------------------------------------------------
               

        UnmapViewOfFile(data);
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------   
    app.Shutdown();
    //--------------------------------------------------------------------------------------

    
    CloseHandle(fileHandle_01);
    CloseHandle(fileHandle_02);

    return 0;
}