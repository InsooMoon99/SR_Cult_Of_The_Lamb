#pragma once
#include "CBoxCollider.h"
#include "CCameraCom.h"
#include "CCombatStat.h"
#include "CComponent.h"
#include "CEnumHelper.h"
#include "CInventory.h"
#include "CPlayerController.h"
#include "CState.h"
#include "CTerrainRenderer.h"
#include "CTransform.h"
#include "CWeaponEquip.h"

USING(Engine)

static void Transform_Inspector(CTransform* transform)
{
    if (!transform)
        return;

    ImGui::Text("[ Transform ]");
    ImGui::PushID("TransformPos");
    {
        _vec3 pos = transform->Get_Pos();

        if (ImGui::DragFloat3("##PositionDrag", &pos.x, 0.1f))
            transform->Set_Pos(pos);
    }
    ImGui::PopID();

    ImGui::NewLine();

    ImGui::Text("[ Rotation ]");

    static _vec3 tempRot;
    static bool  isInit = false;

    if (!isInit)
    {
        tempRot = transform->Get_Rotation();  
        isInit = true;
    }

    ImGui::PushID("TransformRot");
    {
        // X축
        ImGui::Text("Rot X : "); ImGui::SameLine();
        float x = tempRot.x;
        if (ImGui::DragFloat("##Rot X", &x, 0.005f))
        {
            tempRot.x = x;
            transform->Set_RotationAxis(ROT_X, tempRot.x);
        }

        // Y축
        ImGui::Text("Rot Y : "); ImGui::SameLine();
        float y = tempRot.y;
        if (ImGui::DragFloat("##Rot Y", &y, 0.005f))
        {
            tempRot.y = y;
            transform->Set_RotationAxis(ROT_Y, tempRot.y);
        }

        // Z축
        ImGui::Text("Rot Z : "); ImGui::SameLine();
        float z = tempRot.z;
        if (ImGui::DragFloat("##Rot Z", &z, 0.005f))
        {
            tempRot.z = z;
            transform->Set_RotationAxis(ROT_Z, tempRot.z);
        }
    }
    ImGui::PopID();

    ImGui::NewLine();

    ImGui::Text("[ Scale ]");
    ImGui::PushID("TransformScale");
    {
        _vec3 scale = transform->Get_Scale();

        if (ImGui::DragFloat3("##PositionDrag", &scale.x, 0.1f))
            transform->Set_Scale(scale);
    }
    ImGui::PopID();

}

static void State_Inspector(CState* state)
{
    if (state == nullptr)
        return;

    const char* curState = ToString(state->Get_State());
    const char* curDir = ToString(state->Get_Dir());

    ImGui::PushID("State_Inspector");
    {
        // --- State ---
        ImGui::Text("State : ");
        ImGui::SameLine();
        ImGui::TextUnformatted(curState);

        // --- Dir ---
        ImGui::Text("Dir   : ");
        ImGui::SameLine();
        ImGui::TextUnformatted(curDir);
    }
    ImGui::PopID();
}

static void CombatStat_Inspector(CCombatStat* stat)
{
    if (stat == nullptr)
        return;

    _float maxHp = stat->Get_MaxHp();
    _float hp = stat->Get_Hp();

    _float maxMp = stat->Get_MaxMp();
    _float mp = stat->Get_Mp();

    _float speed = stat->Get_Speed();
    _float attack = stat->Get_Attack();

    ImGui::PushID("CombatStat_Inspector");
    {
        const float column_width = 80.0f;

        ImGui::Columns(2, "HpColumns", false);
        ImGui::SetColumnWidth(0, column_width);

        // --- Hp ---
        ImGui::Text("Max Hp : ");
        ImGui::NextColumn();
        if (ImGui::DragFloat("##MaxHp", &maxHp, 0.1f))
        {
            stat->Set_MaxHp(maxHp);
        }
        ImGui::NextColumn();

        ImGui::Text("Hp : ");
        ImGui::NextColumn();
        if(ImGui::DragFloat("##CurHp", &hp, 0.1f))
        {
            stat->Set_Hp(hp);
        }
        ImGui::NextColumn();

        ImGui::Columns(1);

        ImGui::NewLine();

        // --- Mp ---
        ImGui::Columns(2, "MpColumns", false);
        ImGui::SetColumnWidth(0, column_width);

        ImGui::Text("Max Mp : ");
        ImGui::NextColumn();
        if(ImGui::DragFloat("##MaxMp", &maxMp, 0.1f))
        {
            stat->Set_MaxMp(maxMp);
        }
        ImGui::NextColumn();

        ImGui::Text("Mp : ");
        ImGui::NextColumn();
        if(ImGui::DragFloat("##CurMp", &mp, 0.1f))
        {
            stat->Set_Mp(mp);
        }
        ImGui::NextColumn();

        ImGui::Columns(1);

        // --- Speed ---
        ImGui::Columns(2, "EtcColums", false);
        ImGui::SetColumnWidth(0, column_width);

        ImGui::Text("Speed : ");
        ImGui::NextColumn();
        if(ImGui::DragFloat("##Speed", &speed, 0.1f))
        {
            stat->Set_Speed(speed);
        }
        ImGui::NextColumn();

        // --- Attack ---
        ImGui::Text("Attack : ");
        ImGui::NextColumn();
        if(ImGui::DragFloat("##Attack", &attack, 0.1f))
        {
            stat->Set_Attack(attack);
        }
        ImGui::NextColumn();

        ImGui::Columns(1);
    }
    ImGui::PopID();


}

static void Menu_CameraInspector(CCameraCom* camera)
{
    if (camera == nullptr)
        return;

    ImGui::PushID("Main_CameraInspector");
    {
        const char* camModeItems[] = { "Free", "Target" };
        int camMode = camera->Get_CamMode();

        ImGui::SetNextItemWidth(80.f);
        if (ImGui::Combo("##CamMode", &camMode, camModeItems, IM_ARRAYSIZE(camModeItems)))
        {
            camera->Set_CamMode(static_cast<CCameraCom::CAM_MODE>(camMode));
        }
    }
    ImGui::PopID();
}

static void Camera_Inspector(CCameraCom* camera)
{
    if (camera == nullptr)
        return;

    ImGui::PushID("Camera_Inspector");
    {
        ImGui::NewLine();

        _vec3 offset = camera->Get_Offset();

        ImGui::Text("Offset : "); ImGui::SameLine();
        if (ImGui::DragFloat3("##Camera_Offset", &offset.x, 0.001f))
        {
            camera->Set_Offset(offset);
        }

        ImGui::NewLine();
        ImGui::Separator();

        const float column_width = 100.0f;

        // ------------------------
        // CAM_MODE 편집
        // ------------------------
        const char* camModeItems[] = { "Free", "Target" };
        int camMode = camera->Get_CamMode();

        ImGui::Columns(2, "Camera Columns", false);
        ImGui::SetColumnWidth(0, column_width);

        ImGui::Text("Camera Mode"); ImGui::SameLine();
        ImGui::NextColumn();

        if (ImGui::Combo("##Combo Camera Mode", &camMode, camModeItems, IM_ARRAYSIZE(camModeItems)))
        {
            camera->Set_CamMode(static_cast<CCameraCom::CAM_MODE>(camMode));
        }

        ImGui::Columns(1);

        // ------------------------
        // VIEW_TYPE 편집
        // ------------------------
        const char* viewTypeItems[] = { "FPS", "TPS", "Quarter" };
        int         viewType = static_cast<int>(camera->Get_ViewType());

        ImGui::Columns(2, "View Columns", false);
        ImGui::SetColumnWidth(0, column_width);

        ImGui::Text("View Type"); ImGui::SameLine();
        ImGui::NextColumn();

        if (ImGui::Combo("##Combo View Type", &viewType, viewTypeItems, IM_ARRAYSIZE(viewTypeItems)))
        {
            camera->Set_ViewType(static_cast<CCameraCom::VIEW_TYPE>(viewType));
        }

        ImGui::Columns(1);

        // ------------------------
        // PROJ_TYPE 편집
        // ------------------------
        const char* projTypeItems[] = { "Perspective", "Orthographic" };
        int         projType = static_cast<int>(camera->Get_ProjType());

        ImGui::Columns(2, "Project Columns", false);
        ImGui::SetColumnWidth(0, column_width);

        ImGui::Text("Projection"); ImGui::SameLine();
        ImGui::NextColumn();

        if (ImGui::Combo("##Combo Projection Type", &projType, projTypeItems, IM_ARRAYSIZE(projTypeItems)))
        {
            camera->Set_ProjType(static_cast<CCameraCom::PROJ_TYPE>(projType));
        }

        ImGui::Columns(1);

        ImGui::NewLine();
        ImGui::Text(u8"[ 카메라 Free 모드 조작 방법 ]");
        ImGui::NewLine();

        ImGui::Text(u8"[ 이동 ]");
        ImGui::TextWrapped(u8"[I] 앞 | [K] 뒤 | [J] 좌 | [L] 우");
        ImGui::NewLine();

        ImGui::Text(u8"[ 상/하 이동 ]");
        ImGui::TextWrapped(u8"[P] 상 | [;] 하");
        ImGui::NewLine();

        ImGui::Text(u8"[ 회전 ]");
        ImGui::TextWrapped(u8"[LShift + I] 상 | [LShift + I] 하");
        ImGui::TextWrapped(u8"[LShift + J] 좌 | [LShift + L] 우");
    }
    ImGui::PopID();
}

static void Terrain_Inspector(CTerrainRenderer* renderer)
{
    if (renderer == nullptr)
        return;

    if(ImGui::BeginTabBar("Terrain_TabBar"))
    {
        if (ImGui::BeginTabItem(u8"타일"))
        {
            renderer->Render_Editor();

            

            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem(u8"타일 사이즈"))
        {
            ImGui::PushID("Terrain_Resize");
            {
                ImGui::SeparatorText(u8"Terrain 개수 설정");
                ImGui::NewLine();

                static _int resizeX = TILE_CNT_X;
                static _int resizeZ = TILE_CNT_Z;
                static _float cellSize = 1.f;

                const _int textWidth = 65.f;
                const _int boxwidth = 65.f;

                ImGui::Columns(2, nullptr, false);
                ImGui::SetColumnWidth(0, textWidth);

                ImGui::Text("Size X :");
                ImGui::NextColumn();
                ImGui::SetNextItemWidth(boxwidth);
                ImGui::DragInt("##Terrain_SizeX", &resizeX, 1, 10, 129);
                ImGui::NextColumn();

                ImGui::Text("Size Z :");
                ImGui::NextColumn();
                ImGui::SetNextItemWidth(boxwidth);
                ImGui::DragInt("##Terrain_SizeZ", &resizeZ, 1, 10, 129);
                ImGui::NextColumn();
                
                ImGui::Text(u8"셀 크기 :");
                ImGui::NextColumn();
                ImGui::SetNextItemWidth(boxwidth);
                ImGui::DragFloat("##Terrain_CellSize", &cellSize, 0.1f, 0.1f, 10.f);

                ImGui::Columns(1);

                ImGui::NewLine();
                if (ImGui::Button(u8"Terrain 재생성"))
                {
                    renderer->Resize_Terrain(resizeX, resizeZ, cellSize);
                }

                ImGui::NewLine();

                ImGui::SeparatorText(u8"타일 정보");

                ImGui::Text(u8"현재 %d x %d (셀크기: %.1f)",
                    renderer->Get_SizeX(), renderer->Get_SizeZ(), renderer->Get_CellSize());
                
            }
            ImGui::PopID();


            ImGui::EndTabItem();
        }

        ImGui::EndTabBar();
    }
}

static void BoxColl_Inspector(CBoxCollider* collider)
{
    if (collider == nullptr)
        return;

    ImGui::Text("[ Size ]");
    ImGui::PushID("BoxColl_Inspector");
    {
        _vec3 scale = collider->Get_Size();

        if (ImGui::DragFloat3("##PositionDrag", &scale.x, 0.01f))
            collider->Set_Size(scale);

        ImGui::NewLine();

        // Collider Render 그리기
        bool renderCheck = collider->Get_ShowDebug();
        if (ImGui::Checkbox("Render Debug Box", &renderCheck))
        {
            collider->Set_ShowDebug(renderCheck);
        }
        // Collider Active 충돌체크
        bool collisionCheck = collider->IsCollisionActive();
        if (ImGui::Checkbox("Collision Active", &collisionCheck))
        {
            collider->Set_CollisionActive(collisionCheck);
        }

    }
    ImGui::PopID();
    
}

static void Inventory_Inspector(CInventory* inven)
{
    if (inven == nullptr)
        return;

    const vector<InventorySlot>& slots = inven->GetVector();
    int slotCount = (_int)slots.size();

    ImGui::PushID("Inventory_Inspector");
    {
        ImGui::Text("[ Inventory ]");
        ImGui::Text("Slot Count : %d", slotCount);
        ImGui::Spacing();

        if (slotCount == 0)
        {
            ImGui::Text("Empty Inventory");
            return;
        }

        ImGui::Columns(3, "ItemColumns");
        ImGui::Text("Index"); ImGui::NextColumn();
        ImGui::Text("Item"); ImGui::NextColumn();
        ImGui::Text("Count"); ImGui::NextColumn();
        ImGui::Separator();

        for (_int i = 0; i < slotCount; i++)
        {
            const InventorySlot& slot = slots[i];

            // Index
            ImGui::Text("%d", i);
            ImGui::NextColumn();

            // Name
            Item* itemData = nullptr;
            if (slot.itemInst.itemId >= 0)
            {
                itemData = CItemDB::GetInstance()->GetItemById(slot.itemInst.itemId);
            }

            if (itemData)
            {
                ImGui::Text("%s", itemData->name.c_str());

                // 설명 툴팁도 나오도록
                if (ImGui::IsItemHovered())
                {
                    ImGui::SetNextWindowSize(ImVec2(300.f, 0.f)); 

                    ImGui::BeginTooltip();

                    ImGui::PushTextWrapPos(ImGui::GetCursorPos().x + 280.f);  

                    ImGui::TextWrapped("%s", itemData->desc.c_str());
                    if (!itemData->additionalDesc.empty())
                    {
                        ImGui::Separator();
                        ImGui::TextWrapped("%s", itemData->additionalDesc.c_str());
                    }
                    ImGui::EndTooltip();
                }
            }
            else
            {
                ImGui::Text("-");
            }
            ImGui::NextColumn();

            // 4) Count
            if (slot.itemInst.itemId >= 0)
                ImGui::Text("%d", slot.count);
            else
                ImGui::Text("-");
            ImGui::NextColumn();
        }

        ImGui::Columns(1);
    }
    ImGui::PopID();

}

static void Weapon_Inspector(CWeaponEquip* weapon)
{
    if (weapon == nullptr)
        return;

    ImGui::PushID("WeaponEquip_Inspector");
    {
        ImGui::Text("[ Weapon Equip ]");
        ImGui::Separator();

        const InventorySlot& slot = weapon->m_CurrentWeaponSlot;
        int weaponId = weapon->m_CurrentWeaponID;

        if (weaponId < 0 || slot.itemInst.itemId < 0)
        {
            ImGui::Text("Equipped : None");
            ImGui::PopID();
            return;
        }

        Item* itemData = CItemDB::GetInstance()->GetItemById(weaponId);

        if (itemData == nullptr)
        {
            ImGui::Text("Equipped : Invalid Item (ID = %d)", weaponId);
            ImGui::PopID();
            return;
        }

        ImGui::Text("ID    : %d", itemData->id);
        ImGui::Text("Name  : %s", itemData->name.c_str());
        ImGui::Text("Type  : %s", "Weapon");
        ImGui::Text("Count : %d", slot.count);
        ImGui::Spacing();

        ImGui::TextWrapped("%s", itemData->desc.c_str());
        if (!itemData->additionalDesc.empty())
        {
            ImGui::Separator();
            ImGui::TextWrapped("%s", itemData->additionalDesc.c_str());
        }

        ImGui::Spacing();

        // 스탯 출력
        if (!itemData->stats.empty())
        {
            ImGui::Text("Stats");
            ImGui::Separator();

            for (const auto& kv : itemData->stats)
            {
                const std::string& key = kv.first;
                float              value = kv.second;

                ImGui::Text("%s : %.1f", key.c_str(), value);
            }
        }

        ImGui::Spacing();

        // 장착 해제
        if (ImGui::Button("Unequip Weapon"))
        {
            weapon->Unequip_Weapon();
        }
    }
    ImGui::PopID();
}


static void PlayerController_Inspector(CPlayerController* controller)
{
    NULL_CHECK(controller);

    //_float maxHp = controller->Get_MaxHp();
    //_float hp = controller->Get_Hp();
    //
    //_float maxMp = controller->Get_MaxMp();
    //_float mp = controller->Get_Mp();
    //
    //_float speed = controller->Get_Speed();
    //_float attack = controller->Get_Attack();
    //
    //ImGui::PushID("CombatStat_Inspector");
    //{
    //    const float column_width = 80.0f;
    //
    //    ImGui::Columns(2, "HpColumns", false);
    //    ImGui::SetColumnWidth(0, column_width);
    //
    //    // --- Hp ---
    //    ImGui::Text("Max Hp : ");
    //    ImGui::NextColumn();
    //    if (ImGui::DragFloat("##MaxHp", &maxHp, 0.1f))
    //    {
    //        controller->Set_MaxHp(maxHp);
    //    }
    //    ImGui::NextColumn();
    //
    //    ImGui::Text("Hp : ");
    //    ImGui::NextColumn();
    //    if(ImGui::DragFloat("##CurHp", &hp, 0.1f))
    //    {
    //        controller->Set_Hp(hp);
    //    }
    //    ImGui::NextColumn();
    //
    //    ImGui::Columns(1);
    //
    //    ImGui::NewLine();
    //
    //    // --- Mp ---
    //    ImGui::Columns(2, "MpColumns", false);
    //    ImGui::SetColumnWidth(0, column_width);
    //
    //    ImGui::Text("Max Mp : ");
    //    ImGui::NextColumn();
    //    if(ImGui::DragFloat("##MaxMp", &maxMp, 0.1f))
    //    {
    //        controller->Set_MaxMp(maxMp);
    //    }
    //    ImGui::NextColumn();
    //
    //    ImGui::Text("Mp : ");
    //    ImGui::NextColumn();
    //    if(ImGui::DragFloat("##CurMp", &mp, 0.1f))
    //    {
    //        controller->Set_Mp(mp);
    //    }
    //    ImGui::NextColumn();
    //
    //    ImGui::Columns(1);
    //
    //    // --- Speed ---
    //    ImGui::Columns(2, "EtcColums", false);
    //    ImGui::SetColumnWidth(0, column_width);
    //
    //    ImGui::Text("Speed : ");
    //    ImGui::NextColumn();
    //    if(ImGui::DragFloat("##Speed", &speed, 0.1f))
    //    {
    //        controller->Set_Speed(speed);
    //    }
    //    ImGui::NextColumn();
    //
    //    // --- Attack ---
    //    ImGui::Text("Attack : ");
    //    ImGui::NextColumn();
    //    if(ImGui::DragFloat("##Attack", &attack, 0.1f))
    //    {
    //        controller->Set_Attack(attack);
    //    }
    //    ImGui::NextColumn();
    //
    //    ImGui::Columns(1);
    //}
    //ImGui::PopID();

}

static void WallRenderer_Inspector(CWallRenderer* wall)
{
    NULL_CHECK(wall);

    ImGui::Text("1111");

    
}
