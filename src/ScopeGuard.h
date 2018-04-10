#ifndef SRC_SCOPEGUARD_H_
#define SRC_SCOPEGUARD_H_

#include <functional>

namespace OData {
struct ScopeGuard {
  ScopeGuard(std::function<void()> cleanup) : cleanup(std::move(cleanup)) {
  }
  ~ScopeGuard() {
    cleanup();
  }
  std::function<void()> cleanup;
};

} // namespace OData

#endif /* SRC_SCOPEGUARD_H_ */
