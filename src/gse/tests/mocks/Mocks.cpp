#include "Mocks.h"

#include "Test.h"

namespace gse {
namespace tests {
namespace mocks {

void AddMocks( context::GlobalContext* ctx, const test_info_t& test_info ) {
	Test::AddMocks( ctx, test_info );
}

}
}
}
