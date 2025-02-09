# Add external project as sysbuild image for the Network Core
ExternalZephyrProject_Add(
  APPLICATION NetworkCore
  SOURCE_DIR ${APP_DIR}/../NetworkCore
  BOARD thingy53/nrf5340/cpunet
)