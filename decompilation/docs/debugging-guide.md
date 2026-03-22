# ForgedAlliance.exe RAM Debugging with Ghidra

Since you are using **Ghidra**, you have the huge advantage of combining static analysis (decompilation) and dynamic analysis (live debugging) in a single tool! 

Ghidra features an extremely powerful integrated debugger (starting from version 10.x).

## 1. Setting up the Ghidra Debugger (Windows)

On Windows, Ghidra internally utilizes Microsoft's debugging tools (`dbgeng.dll` or `gdb` via MinGW/Cygwin).
The easiest method is to use the "IN-VM dbgeng" connector.

1. Open your `ForgedAlliance.exe` project in the Ghidra CodeBrowser.
2. Click on the **Bug Icon (Debugger)** in the menu bar to switch to the Debugger view.
3. In the "Debugger Targets" window (usually on the left), click the **"Connect"** icon (plug symbol).
4. Choose **`dbgeng` (IN-VM)** as your target.
5. Click either the **"Launch"** icon (rocket) OR the **"Attach"** icon (two rings):
   * **Attach**: Start FA normally (in windowed mode via `/windowed 1024x768`), click *Attach* in Ghidra, and select the running `ForgedAlliance.exe` process from the list. (Recommended!)
   * **Launch**: You can let Ghidra start the EXE directly.

## 2. Setting Breakpoints (Live Analysis)

As soon as Ghidra is attached, the game will be paused. You will now see the live memory in the "Dynamic Listing" (on the right).

1. Go to a function we already discovered in your normal "Static Listing" (on the left), e.g., `0x00894A10` (`CWldSession_DoBeat`).
2. Right-click the first assembly instruction -> **Add Breakpoint -> SW_EXECUTE** (Software Breakpoint).
3. Switch back to the Debugger's "Objects" window or use the controls at the top to click the **"Resume"** (Play Button) icon. FA will resume running.
4. The moment the next beat is calculated in the game, **the breakpoint will trigger**. Ghidra will pause the FA process again.

## 3. Evaluating the Call Stack (Stack Trace)

This is the most important method for finding the missing inlined functions!

1. When Ghidra is paused at the breakpoint in `DoBeat`, open the **"Stack"** window (usually at the bottom right of the Debugger layout).
2. Here you can see the call hierarchy:
   ```
   [0] 0x00894A10 (CWldSession_DoBeat)
   [1] 0x00XXXXXX (The unknown function that called DoBeat!)
   [2] 0x00YYYYYY (WLD_Frame / Main Loop?)
   ```
3. Double-click on frame `[1]`. Ghidra will instantly jump to this address.
4. You can immediately **label** this previously unknown address in the Static Listing (e.g., as `CMarshaller_AdvanceBeat` or whoever made the call) and use the decompiler to analyze it.

## 4. Viewing Memory Objects (VTables) Live

Often we don't have the callers, but we do know the VTable offsets from the `.dll` (e.g., `AdvanceBeat` is at offset `0x34` of the `Sim` VTable).

1. Pause the game in Ghidra.
2. Go to the "Registers" window and look for the `ECX` register (in the Visual C++ `__thiscall` convention, the "this" pointer of the called object is almost always stored in `ECX`).
3. Right-click the `ECX` value -> **"Go To in Dynamic Listing"**.
4. You will now see the raw C++ object (e.g., `CWldSession`) live in RAM.
5. The **first 4 bytes** of this object serve as the pointer to its VTable.
6. Right-click these first 4 bytes in the Dynamic Listing -> **"Follow Pointer"**.
7. Bam! You have landed in the `.rdata` section of `ForgedAlliance.exe` and are staring at a massive list of function pointers. These are **all the inlined virtual functions** of this specific class.
8. Now you simply need to apply the offsets obtained from the DLL analysis (function 1 = `+0x00`, function 2 = `+0x04`, etc.), and you can immediately label and name all these addresses as functions in Ghidra!

## Tips for Ghidra Debugging
* **Module Base Address:** Ensure Ghidra properly maps your Static Image to the running EXE's base address (usually `0x00400000`). This typically happens automatically upon attaching.
* If the game crashes, try using windowed mode or keep the pause times very short, as network timeouts might trigger.
