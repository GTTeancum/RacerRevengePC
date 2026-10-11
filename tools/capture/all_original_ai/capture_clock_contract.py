"""Source-derived render/native clock contract; no process or file operations."""


def verify_capture_clocks(frame, item):
    # captureCommitted emits one complete image ACK, then capture_state from
    # lastCommittedTick, then host_ui_state from completedFrames. These counters
    # have distinct ownership. A menu redraw never commits a new gameplay tick.
    state = frame.get('capture_state')
    if (not isinstance(state, dict)
            or any(type(value) is not int or value < 0 for value in
                   (frame.get('tick'), frame.get('rendered_tick'), state.get('rendered_tick'), item.get('tick')))
            or type(state.get('host_menu_image')) is not bool
            or type(item.get('menu')) is not bool
            or frame['tick'] != frame['rendered_tick']
            or state['rendered_tick'] != frame['rendered_tick']
            or state['host_menu_image'] != item['menu']):
        raise RuntimeError('Image ACK, capture_state, and UI image-kind association disagree')
    rendered, native = frame['rendered_tick'], item['tick']
    if rendered > native:
        raise RuntimeError('Rendered scene tick is ahead of the completed native counter')
    if not item['menu']:
        # Normal live draws assign lastCommittedTick=completedFrames immediately
        # before captureCommitted. A retained recovery image is not live video.
        if frame.get('retained_before_undertrack_pause') or rendered != native:
            raise RuntimeError('Live-scene capture lacks the exact freshly rendered native tick')
    frame['native_tick'] = native
    frame['tick_relation'] = 'menu_retains_scene_tick' if item['menu'] else 'fresh_scene_equals_native'

