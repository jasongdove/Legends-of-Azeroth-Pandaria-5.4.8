/*
* This file is part of the Pandaria 5.4.8 Project. See THANKS file for Copyright information
*
* This program is free software; you can redistribute it and/or modify it
* under the terms of the GNU General Public License as published by the
* Free Software Foundation; either version 2 of the License, or (at your
* option) any later version.
*
* This program is distributed in the hope that it will be useful, but WITHOUT
* ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
* FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
* more details.
*
* You should have received a copy of the GNU General Public License along
* with this program. If not, see <http://www.gnu.org/licenses/>.
*/

#include "ScriptMgr.h"
#include "InstanceScript.h"
#include "ScriptedCreature.h"
#include "shadow_labyrinth.h"

static std::vector<DoorData> const doorData =
{
    { GO_REFECTORY_DOOR,        DATA_BLACKHEART_THE_INCITER,    DOOR_TYPE_PASSAGE,  BOUNDARY_NONE },
    { GO_SCREAMING_HALL_DOOR,   DATA_GRANDMASTER_VORPIL,        DOOR_TYPE_PASSAGE,  BOUNDARY_NONE },
};

class instance_shadow_labyrinth : public InstanceMapScript
{
    public:
        instance_shadow_labyrinth() : InstanceMapScript(SLScriptName, 555) { }

        struct instance_shadow_labyrinth_InstanceMapScript : public InstanceScript
        {
            instance_shadow_labyrinth_InstanceMapScript(Map* map) : InstanceScript(map)
            {
                SetBossNumber(EncounterCount);
                LoadDoorData(doorData);

                AmbassadorHellmawGUID = ObjectGuid::Empty;
                GrandmasterVorpilGUID = ObjectGuid::Empty;
                FelOverseerCount      = 0;
            }

            void OnCreatureCreate(Creature* creature) override
            {
                switch (creature->GetEntry())
                {
                    case NPC_AMBASSADOR_HELLMAW:
                        AmbassadorHellmawGUID = creature->GetGUID();
                        break;
                    case NPC_GRANDMASTER_VORPIL:
                        GrandmasterVorpilGUID = creature->GetGUID();
                        break;
                    case NPC_FEL_OVERSEER:
                        if (creature->IsAlive())
                        {
                            ++FelOverseerCount;
                            if (Creature* hellmaw = instance->GetCreature(AmbassadorHellmawGUID))
                                hellmaw->AI()->DoAction(ACTION_AMBASSADOR_HELLMAW_BANISH);
                        }
                        break;
                    default:
                        break;
                }
            }

            void OnGameObjectCreate(GameObject* go) override
            {
                switch (go->GetEntry())
                {
                    case GO_REFECTORY_DOOR:
                    case GO_SCREAMING_HALL_DOOR:
                        AddDoor(go, true);
                        break;
                    default:
                        break;
                }
            }

            void OnUnitDeath(Unit* unit) override
            {
                Creature* creature = unit->ToCreature();
                if (!creature)
                    return;

                if (creature->GetEntry() == NPC_FEL_OVERSEER)
                {
                    if (FelOverseerCount)
                        --FelOverseerCount;

                    if (!FelOverseerCount)
                        if (Creature* hellmaw = instance->GetCreature(AmbassadorHellmawGUID))
                            hellmaw->AI()->DoAction(ACTION_AMBASSADOR_HELLMAW_INTRO);
                }
            }

            uint32 GetData(uint32 type) const override
            {
                switch (type)
                {
                    case DATA_FEL_OVERSEER:
                        return !FelOverseerCount ? 1 : 0;
                    default:
                        break;
                }
                return 0;
            }

            ObjectGuid GetGuidData(uint32 type) const override
            {
                switch (type)
                {
                    case DATA_GRANDMASTER_VORPIL:
                        return GrandmasterVorpilGUID;
                    default:
                        break;
                }
                return ObjectGuid::Empty;
            }

            std::string GetSaveData() override
            {
                OUT_SAVE_INST_DATA;

                std::ostringstream saveStream;
                saveStream << "S L " << GetBossSaveData();

                OUT_SAVE_INST_DATA_COMPLETE;
                return saveStream.str();
            }

            void Load(char const* str) override
            {
                if (!str)
                {
                    OUT_LOAD_INST_DATA_FAIL;
                    return;
                }

                OUT_LOAD_INST_DATA(str);

                char dataHead1, dataHead2;

                std::istringstream loadStream(str);
                loadStream >> dataHead1 >> dataHead2;

                if (dataHead1 == 'S' && dataHead2 == 'L')
                {
                    for (uint32 i = 0; i < EncounterCount; ++i)
                    {
                        uint32 tmpState;
                        loadStream >> tmpState;
                        if (tmpState == IN_PROGRESS || tmpState > SPECIAL)
                            tmpState = NOT_STARTED;
                        SetBossState(i, EncounterState(tmpState));
                    }
                }
                else
                    OUT_LOAD_INST_DATA_FAIL;

                OUT_LOAD_INST_DATA_COMPLETE;
            }

        protected:
            ObjectGuid AmbassadorHellmawGUID;
            ObjectGuid GrandmasterVorpilGUID;
            uint32 FelOverseerCount;
        };

        InstanceScript* GetInstanceScript(InstanceMap* map) const override
        {
            return new instance_shadow_labyrinth_InstanceMapScript(map);
        }
};

void AddSC_instance_shadow_labyrinth()
{
    new instance_shadow_labyrinth();
}
