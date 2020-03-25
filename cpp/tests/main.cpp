#define APPROVALS_DOCTEST
#include "ApprovalTests.hpp"

#include <memory>
auto defaultReporterDisposer =
        Approvals::useAsDefaultReporter(std::make_shared<ClipboardReporter>());
