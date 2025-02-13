#include "bpforc.h"

namespace bpftrace {

using namespace llvm;
using namespace llvm::orc;

uint8_t *MemoryManager::allocateCodeSection(uintptr_t Size,
                                            unsigned Alignment,
                                            unsigned SectionID,
                                            StringRef SectionName)
{
  uint8_t *addr = SectionMemoryManager::allocateCodeSection(
      Size, Alignment, SectionID, SectionName);
  sections_[SectionName.str()] = std::make_tuple(addr, Size);
  return addr;
}

uint8_t *MemoryManager::allocateDataSection(uintptr_t Size,
                                            unsigned Alignment,
                                            unsigned SectionID,
                                            StringRef SectionName,
                                            bool isReadOnly)
{
  uint8_t *addr = SectionMemoryManager::allocateDataSection(
      Size, Alignment, SectionID, SectionName, isReadOnly);
  sections_[SectionName.str()] = std::make_tuple(addr, Size);
  return addr;
}

std::unordered_map<std::string, std::vector<uint8_t>> BpfOrc::getBytecode()
    const
{
  std::unordered_map<std::string, std::vector<uint8_t>> ret;

  for (const auto &[section, bytecode_tuple] : sections_)
  {
    std::vector<uint8_t> section_bytecode;
    const auto &[ptr, size] = bytecode_tuple;
    section_bytecode.reserve(size);
    section_bytecode.assign(ptr, ptr + size);
    ret.insert({ section, section_bytecode });
  }

  return ret;
}

std::optional<std::tuple<uint8_t *, uintptr_t>> BpfOrc::getSection(
    const std::string &name)
{
  auto sec = sections_.find(name);
  if (sec == sections_.end())
    return std::nullopt;
  return sec->second;
}

#ifdef LLVM_ORC_V1
#include "bpforcv1.cpp"
#else // LLVM_ORC_V2
#include "bpforcv2.cpp"
#endif

} // namespace bpftrace
