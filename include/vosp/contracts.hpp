#pragma once

/** @file contracts.hpp Public umbrella for MicroContractsFramework. */

#include <vosp/contracts/cache.hpp>
#include <vosp/contracts/configuration.hpp>
#include <vosp/contracts/error.hpp>
#include <vosp/contracts/logging.hpp>
#include <vosp/contracts/plugin.hpp>
#include <vosp/contracts/protocol.hpp>
#include <vosp/contracts/security.hpp>
#include <vosp/contracts/telemetry.hpp>
#include <vosp/contracts/transport.hpp>
#include <vosp/contracts/version.hpp>

#ifndef VOSP_NAMESPACE_FACADE_DEFINED
#define VOSP_NAMESPACE_FACADE_DEFINED
/** @brief Compact namespace facade shared by the VOSP ecosystem. */
namespace vsp = vosp;
#endif
