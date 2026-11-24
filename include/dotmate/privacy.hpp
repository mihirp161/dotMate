#pragma once

#include <cstdint>
#include <string>

namespace dotmate::privacy {

// Non-cryptographic (FNV-1a) hash of a normalized name, hex-encoded.
// This is NOT for security -- it's for the narrow, low-stakes job of
// remembering "someone named X was here before" without writing their
// real name to disk. Do not use this for anything that needs to resist
// a determined attacker (passwords, auth tokens, etc.).
//
// This is the entire mechanism behind the "welcome back" feature: by
// design, it never touches what a user actually asked. The original
// project wrote the typed name in plain text and logged every question
// and answer to a per-name file, forever, with no opt-out -- that's
// what this replaces.
std::string pseudonymize(const std::string& name);

}  // namespace dotmate::privacy
