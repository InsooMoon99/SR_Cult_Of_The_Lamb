# Camera System Refactoring Plan

## Goal Description
Refactor the camera system into a modular architecture with clear separation of concerns between Engine (Mechanism) and Client (Policy).

## Architecture Overview

### Engine (Reusable Core)
-   **`CCameraCalc`** (Renamed from [CCameraCom](file:///d:/cpp_ksh/Jusin158/DX9/DX9_SR_Portfolio/SR_Cult/Engine/Header/CCameraCom.h#40-41)):
    -   **Role**: **The Lens & Sensor**.
    -   **Responsibility**: Purely calculates View & Projection matrices based on the owner's Transform. Does **not** handle input or movement.
    -   **Why Engine?**: Matrix calculation is universal math required for any 3D game.

### Client (Game Specific)
-   **`CCameraController`** (New):
    -   **Role**: **The Brain**.
    -   **Responsibility**: Handles User Input, manages Camera States (Free Mode vs Target Mode), and decides *which* movement to apply.
    -   **Why Client?**: Input mapping and state logic (e.g., "Switch to Target Mode when Player spawns") are specific to this game.

-   **`CCameraMovement`** (New):
    -   **Role**: **The Legs**.
    -   **Responsibility**: Implements specific movement algorithms.
        -   `CamMove_FreeMode`: WASD + Mouse Look.
        -   `CamMove_Target`: Follow Target with Offset (Spherical/Cartesian).
    -   **Why Client?**: Movement "feel" (smoothing, collision, speed) is often game-specific.

-   **[CMainCamera](file:///d:/cpp_ksh/Jusin158/DX9/DX9_SR_Portfolio/SR_Cult/Client/Code/CMainCamera.cpp#17-23)**:
    -   **Role**: **The Container**.
    -   **Responsibility**: Owns the above components and assembles them into a functional Game Object.

## Proposed Changes

### Engine

#### [RENAME & MODIFY] [CCameraCom](file:///d:/cpp_ksh/Jusin158/DX9/DX9_SR_Portfolio/SR_Cult/Engine/Header/CCameraCom.h#40-41) -> `CCameraCalc`
-   **File**: `Engine/Header/CCameraCalc.h`, `Engine/Code/CCameraCalc.cpp`
-   **Changes**:
    -   Rename class to `CCameraCalc`.
    -   Remove `Update/LateUpdate`.
    -   Remove `m_CamMode`, `m_ViewType`, `m_Offset`.
    -   Add [Compute_View(CTransform*)](file:///d:/cpp_ksh/Jusin158/DX9/DX9_SR_Portfolio/SR_Cult/Engine/Code/CCameraCom.cpp#157-175) and [Compute_Proj(...)](file:///d:/cpp_ksh/Jusin158/DX9/DX9_SR_Portfolio/SR_Cult/Engine/Code/CCameraCom.cpp#186-190).

### Client

#### [NEW] `CCameraMovement`
-   **File**: `Client/Header/CCameraMovement.h`, `Client/Code/CCameraMovement.cpp`
-   **Inherits**: [CComponent](file:///d:/cpp_ksh/Jusin158/DX9/DX9_SR_Portfolio/SR_Cult/Engine/Code/CComponent.cpp#6-14)
-   **Functions**:
    -   `void CamMove_FreeMode(CTransform* pTransform, float fTimeDelta)`
    -   `void CamMove_Target(CTransform* pTransform, CTransform* pTarget, const _vec3& vOffset)`

#### [NEW] `CCameraController`
-   **File**: `Client/Header/CCameraController.h`, `Client/Code/CCameraController.cpp`
-   **Inherits**: [CComponent](file:///d:/cpp_ksh/Jusin158/DX9/DX9_SR_Portfolio/SR_Cult/Engine/Code/CComponent.cpp#6-14)
-   **Members**:
    -   `CAM_MODE` (Free, Target).
    -   `m_Offset` (Vector3).
    -   `CCameraMovement* m_MovementCom`.
-   **Update**:
    -   Process Input.
    -   Call appropriate function in `m_MovementCom`.

#### [MODIFY] [CMainCamera](file:///d:/cpp_ksh/Jusin158/DX9/DX9_SR_Portfolio/SR_Cult/Client/Code/CMainCamera.cpp#17-23)
-   **Changes**:
    -   Add `CCameraController`, `CCameraMovement`, `CCameraCalc`.
    -   [LateUpdate](file:///d:/cpp_ksh/Jusin158/DX9/DX9_SR_Portfolio/SR_Cult/Client/Code/CEdit.cpp#54-60): Call `CCameraCalc::Compute_View` and set Device Transform.
    -   [Render_Editor](file:///d:/cpp_ksh/Jusin158/DX9/DX9_SR_Portfolio/SR_Cult/Client/Code/CMainCamera.cpp#66-72): Expose `CCameraController` settings.

## Verification Plan
1.  **Refactor Engine**: Rename [CCameraCom](file:///d:/cpp_ksh/Jusin158/DX9/DX9_SR_Portfolio/SR_Cult/Engine/Header/CCameraCom.h#40-41) to `CCameraCalc` and strip logic.
2.  **Implement Client Components**: Create `CCameraMovement` and `CCameraController`.
3.  **Assemble**: Update [CMainCamera](file:///d:/cpp_ksh/Jusin158/DX9/DX9_SR_Portfolio/SR_Cult/Client/Code/CMainCamera.cpp#17-23) to use the new components.
4.  **Verify**: Check Free Mode and Target Mode in-game.
