#include <DUNE/DUNE.hpp>

namespace Tutorials
{
    //! Simple task that produces random temperature measurements.
    namespace TempProd
    {
        using DUNE_NAMESPACES;

        //!Task arguments.
        struct Arguments
        {
            //! PRNG type.
            std::string prng_type;
            //! PRNG seed.
            int prng_seed;
            //! Mean temperature value.
            float mean_value;
            //! Standard deviation of temperature measurements.
            double std_dev;
        };

        struct Task: public DUNE::Tasks::Periodic
        {
            //! PRNG handle
            Random::Generator* m_prng;
            //! Task arguments.
            Arguments m_args;

            Task(const std::string& name, Tasks::Context& ctx):
                    DUNE::Tasks::Periodic(name, ctx),
                    m_prng(NULL)
            {
                param("Standard deviation", m_args.std_dev)
                        .description("Standard deviation of produced temperature")
                        .units(Units::DegreeCelsius)
                        .defaultValue("0.1");

                param("PRNG Type", m_args.prng_type)
                        .defaultValue(Random::Factory::c_default);

                param("PRNG Seed", m_args.prng_seed)
                        .defaultValue("-1");

                param("Mean value", m_args.mean_value)
                        .description("Mean value of produced temperature")
                        .units(Units::DegreeCelsius)
                        .defaultValue("25.0");
            }

            void
            onEntityReservation(void)
            {
                inf("Starting: %s", resolveEntity(getEntityId()).c_str());
            }

            //! Aquire resources.
            void
            onResourceAcquisition(void)
            {
                m_prng = Random::Factory::create(m_args.prng_type,
                                                 m_args.prng_seed);
            }

            //! Release resources.
            void
            onResourceRelease(void)
            {
                Memory::clear(m_prng);
            }


            //! Periodic work.
            void
            task(void)
            {
                IMC::Temperature temperature;
                temperature.value = m_args.mean_value + m_prng->gaussian()*m_args.std_dev;
                temperature.setSourceEntity(getEntityId());
                dispatch(temperature);
            }
        };
    }
}

DUNE_TASK