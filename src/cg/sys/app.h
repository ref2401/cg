#ifndef CG_SYS_APP_H_
#define CG_SYS_APP_H_

#include <cassert>
#include <memory>
#include <windows.h>
#include "cg/math/math.h"


namespace cg {
namespace sys {

class IGame {
public:

	virtual ~IGame() noexcept = default;


	virtual void on_window_resize() = 0;

	virtual void render(float blend_state) = 0;

	virtual void update(float dt) = 0;
};

class IWindow {
public:

	virtual void show() noexcept = 0;

	// Returns the window's size in pixels.
	virtual uint2 size() const noexcept = 0;

protected:
	~IWindow() noexcept = default;
};

class IApplication {
public:

	virtual ~IApplication() noexcept = default;


	virtual void run(std::unique_ptr<IGame> game) = 0;

	virtual IWindow* window() const noexcept = 0;
};

std::unique_ptr<IApplication> make_win32_application(uint2 wnd_position, uint2 wnd_size);

} // namespace sys
} // namespace cg

#endif // CG_SYS_APP_H_
