//***************************************************************************
// Copyright 2007-2022 Universidade do Porto - Faculdade de Engenharia      *
// Laboratório de Sistemas e Tecnologia Subaquática (LSTS)                  *
//***************************************************************************
// This file is part of DUNE: Unified Navigation Environment.               *
//                                                                          *
// Commercial Licence Usage                                                 *
// Licencees holding valid commercial DUNE licences may use this file in    *
// accordance with the commercial licence agreement provided with the       *
// Software or, alternatively, in accordance with the terms contained in a  *
// written agreement between you and Faculdade de Engenharia da             *
// Universidade do Porto. For licensing terms, conditions, and further      *
// information contact lsts@fe.up.pt.                                       *
//                                                                          *
// Modified European Union Public Licence - EUPL v.1.1 Usage                *
// Alternatively, this file may be used under the terms of the Modified     *
// EUPL, Version 1.1 only (the "Licence"), appearing in the file LICENCE.md *
// included in the packaging of this file. You may not use this work        *
// except in compliance with the Licence. Unless required by applicable     *
// law or agreed to in writing, software distributed under the Licence is   *
// distributed on an "AS IS" basis, WITHOUT WARRANTIES OR CONDITIONS OF     *
// ANY KIND, either express or implied. See the Licence for the specific    *
// language governing permissions and limitations at                        *
// https://github.com/LSTS/dune/blob/master/LICENCE.md and                  *
// http://ec.europa.eu/idabc/eupl.html.                                     *
//***************************************************************************
// Author: DuneAuthor                                                       *
//***************************************************************************

// DUNE headers.
#include <DUNE/DUNE.hpp>
#include <vector>
#include <utility>
namespace Maneuver
{
  //! Insert short task description here.
  //!
  //! Insert explanation on task behaviour here.
  //! @author DuneAuthor
  namespace test_task
  {
    using DUNE_NAMESPACES;

    struct Task: public DUNE::Tasks::Task
    {
      //! Constructor.
      //! @param[in] name task name.
      //! @param[in] ctx context.
      Task(const std::string& name, Tasks::Context& ctx):
        DUNE::Tasks::Task(name, ctx)
      {
      }

      //! Update internal state with new parameter values.
      void
      onUpdateParameters(void)
      {
      }

      //! Reserve entity identifiers.
      void
      onEntityReservation(void)
      {
      }

      //! Resolve entity names.
      void
      onEntityResolution(void)
      {
      }

      //! Acquire resources.
      void
      onResourceAcquisition(void)
      {
      }

      //! Initialize resources.
      void
      onResourceInitialization(void)
      {
      }

      //! Release resources.
      void
      onResourceRelease(void) {
      }
          //! This (utility) method generates a PlanSpecification
          //! consisting in the given maneuver sequence.
          //! @param[in] plan_id The name of the plan to be generated
          //! @param[in] maneuvers A vector with maneuvers (order of the resulting plan will correspond
          //! to the order of this vector).
          //! @param[out] result The resulting PlanSpecification will be stored here.
          void
          sequentialPlan(std::string plan_id, const IMC::MessageList<IMC::Maneuver>* maneuvers, IMC::PlanSpecification& result)
          {
              IMC::PlanManeuver last_man;

              IMC::MessageList<IMC::Maneuver>::const_iterator itr;
              unsigned i = 0;
              for (itr = maneuvers->begin(); itr != maneuvers->end(); itr++, i++)
              {
                  if (*itr == NULL)
                      continue;

                  IMC::PlanManeuver man_spec;

                  man_spec.data.set(*(*itr));
                  man_spec.maneuver_id = String::str(i + 1);
                  if (itr == maneuvers->begin())
                  {
                      // no transitions.
                  }
                  else
                  {
                      IMC::PlanTransition trans;
                      trans.conditions = "ManeuverIsDone";
                      trans.dest_man = man_spec.maneuver_id;
                      trans.source_man = last_man.maneuver_id;

                      result.transitions.push_back(trans);
                  }

                  result.maneuvers.push_back(man_spec);

                  last_man = man_spec;
              }

              result.plan_id = plan_id;
              result.start_man_id = "1";
          }


        DUNE::IMC::PlanDB
        createPlanDBEntry (std::vector<std::pair<double, double>> path, std::string plan_id, fp32_t speed) {

            DUNE::IMC::MessageList<DUNE::IMC::Maneuver> maneuvers; //Define list of meneuvers

// Make maneuvers

            for(auto p: path ) {

                //const auto state= static_cast<const ompl::base::RealVectorStateSpace::StateType *>(paths.getState(i));

                DUNE::IMC::Goto* go_near = new DUNE::IMC::Goto();

                go_near->lat = DUNE::Math::Angles::radians(p.first);

                go_near->lon = DUNE::Math::Angles::radians(p.second);

                go_near->speed_units = DUNE::IMC::SUNITS_METERS_PS;

                go_near->speed = speed;//m_args.speed_rpms;

                maneuvers.push_back(*go_near);

                delete go_near;
            }

            DUNE::IMC::PlanSpecification pspec;

            sequentialPlan(plan_id, &maneuvers, pspec);

            DUNE::IMC::PlanDB pdb;

            pdb.op = DUNE::IMC::PlanDB::DBOP_SET;

            pdb.type = DUNE::IMC::PlanDB::DBT_REQUEST;

            pdb.plan_id = pspec.plan_id;

            pdb.arg.set(pspec);

            pdb.request_id = 0;

            return pdb;

        }
        void activatePlan(std::string plan_id, uint16_t id)  {
            bool ignore_errors = true;
            IMC::PlanControl pcontrol;
            pcontrol.type = IMC::PlanControl::PC_REQUEST;
            pcontrol.op = IMC::PlanControl::PC_START;
            pcontrol.plan_id = plan_id;
            pcontrol.setDestination(m_ctx.resolver.id());
            if (ignore_errors)
                pcontrol.flags = IMC::PlanControl::FLG_IGNORE_ERRORS;
            //pcontrol.setDestination(id);
            dispatch(pcontrol);

            spew("Plan start request sent");
        }
      //! Main loop.
      void
      onMain(void)
      {
          std::vector<std::pair<double, double>> path = {{41.1861304,-8.70793785},{41.1861082,-8.70654042}};
          DUNE::IMC::PlanDB x = createPlanDBEntry(path,"p1",1.5);
          x.setDestination(0x2810);
          dispatch(x);
          activatePlan("p1",0x2810);
        while (!stopping())
        {

            waitForMessages(1.0);
        }
      }
    };
  }
}

DUNE_TASK
