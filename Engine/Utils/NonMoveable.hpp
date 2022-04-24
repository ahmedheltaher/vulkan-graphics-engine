#pragma once 


struct NonMoveable {
	NonMoveable() = default;
	NonMoveable(NonMoveable&&) = delete;
	NonMoveable& operator=(const NonMoveable&&) = delete;
};
