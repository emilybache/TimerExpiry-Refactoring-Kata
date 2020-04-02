#define APPROVALS_CATCH
#include "ApprovalTests.hpp"

#include <memory>

auto defaultReporterDisposer =
        ApprovalTests::Approvals::useAsDefaultReporter(std::make_shared<ApprovalTests::ClipboardReporter>());

//auto frontLoadedReportDisposer = Approvals::Approvals::useAsFrontLoadedReporter(
//        BlockingReporter::onMachineNamed("Emily Bache’s MacBook Pro"));