#include <Windows.h>
#include <iostream>
#include <vector>

#include "Memory.h"
#include "Offsets.h"

std::string getName(uintptr_t address) // this is a very simple get name function you can just make it better but this is good for now
{
    uintptr_t namePtr = read<uintptr_t>(address + offsets::Name);

    std::string name;

    if (read<int>(namePtr + 0x10) >= 16)
    {
        uintptr_t namePtr2 = read<uintptr_t>(namePtr);
        
        while (read<char>(namePtr2) != '\0')
        {
            name += read<char>(namePtr2);
            namePtr2++;
        }
    }
    else
    {
        name = read<std::string>(namePtr);
    }
    
    return name;
}

std::vector<uintptr_t> getChildren(uintptr_t address) // also this is a very simple get children function because every function in this project will be simple
{
    std::vector<uintptr_t> children;

    uintptr_t start = read<uintptr_t>(address + offsets::Children);
    uintptr_t end = read<uintptr_t>(start + offsets::ChildrenEnd);

    for (auto i = read<uintptr_t>(start); i < end; i += 0x10)
    {
        children.push_back(read<uintptr_t>(i));
    }

    return children;
}

uintptr_t findFirstChild(uintptr_t address, std::string name) // yes you guessed it! this is also a very simple function
{
    for (auto child : getChildren(address))
    {
        if (getName(child) == name)
        {
            return child;
        }
    }

    return 0;
}

int main()
{
    openHandle(); // open handle

    uintptr_t robloxBase = getModuleBaseAddress("RobloxPlayerBeta.exe"); // get roblox base address

    uintptr_t fakeDataModel = read<uintptr_t>(robloxBase + offsets::FakeDataModelPointer); // get fake data model
    uintptr_t realDataModel = read<uintptr_t>(fakeDataModel + offsets::FakeDataModelToDataModel); // get real data model from fake data model

    uintptr_t playersAddr = findFirstChild(realDataModel, "Players"); // get players address from data model with iterating through all of the data model's children. you can make one that checks the class name but for now we don't need it
    
    uintptr_t localPlayer = read<uintptr_t>(playersAddr + offsets::LocalPlayer); // get local player from players address
    uintptr_t character = read<uintptr_t>(localPlayer + offsets::ModelInstance); // get character from local player address
    uintptr_t humanoid = findFirstChild(character, "Humanoid"); // get humanoid from character address

    write<float>(humanoid + offsets::WalkSpeed, 50.0f); // now fianlly we set the walkspeed!
    write<float>(humanoid + offsets::WalkSpeedCheck, 50.0f); // we need to write to both walkspeed and walkspeedcheck offsets because roblox checks both of them

    write<float>(humanoid + offsets::JumpPower, 200.0f); // like this way we can change the jump power too!

    std::cout << "Modified walkspeed and jumppower!" << std::endl;

    system("pause");
    return 0;
}
