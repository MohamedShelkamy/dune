#include <DUNE/DUNE.hpp>

namespace Tutorials
{
    namespace TempCons
    {
        using DUNE_NAMESPACES;

        struct Task: public DUNE::Tasks::Task
        {
            // Parameters.
            std::string m_trg_prod;

            Task(const std::string& name, Tasks::Context& ctx):
                    DUNE::Tasks::Task(name, ctx)
            {
                param("Target Producer", m_trg_prod)
                        .description("Target producer to read from")
                        .defaultValue("Producer");

                bind<IMC::Temperature>(this);
            }

            void
            consume(const IMC::Temperature* msg)
            {
                 //if (m_trg_prod == msg.get(SourceEntity))
                 inf("Source (DUNE instance) ID is: %d", msg->getSource());
                 inf("Source entity (Task instance) ID is: %d", msg->getSourceEntity());
                 inf("Temperature is %f, from %s", msg->value, resolveEntity(msg->getSourceEntity()).c_str());

            }

            void
            onMain(void)
            {
                while (!stopping())
                {
                    waitForMessages(1.0);
                }
            }
        };
    }
}
DUNE_TASK