function ncc_mask
% ncc VOT integration example
%
% This function is an example of tracker integration into the toolkit.
% The implemented tracker is a very simple NCC tracker that is also used as
% the baseline tracker for challenge entries.
%

% *************************************************************
% VOT: Set random seed to a different value every time.
% *************************************************************
try
    % Simple check for Octave environment
    OCTAVE_VERSION;
    rand('seed', sum(clock));
    pkg load image;
catch
    RandStream.setGlobalStream(RandStream('mt19937ar', 'Seed', sum(clock)));
end

% **********************************
% VOT: Get initialization data
% **********************************
[handle, image, region] = vot('mask');

img = imread(image);

mask = make_full_size(region, size(img, 2), size(img, 1));

% Initialize the tracker
[state, ~] = ncc_initialize(img, mask);

while true

    % **********************************
    % VOT: Get next frame
    % **********************************
    [handle, image] = handle.frame(handle);

    if isempty(image)
        break;
    end;

	% Perform a tracking step, obtain new region
    [state, mask_predicted, confidence] = ncc_update(state, imread(image));
    
    % **********************************
    % VOT: Report position for frame
    % **********************************
    handle = handle.report(handle, uint8(mask_predicted), confidence);

end;

% **********************************
% VOT: Output the results
% **********************************
handle.quit(handle);

end

function [state, location] = ncc_initialize(I, mask, varargin)
    
    region = rect_from_mask(mask);
    
    gray = double(rgb2gray(I));

    [height, width] = size(gray);

    % If the provided region is a polygon ...
    if numel(region) > 4
        x1 = round(min(region(1:2:end)));
        x2 = round(max(region(1:2:end)));
        y1 = round(min(region(2:2:end)));
        y2 = round(max(region(2:2:end)));
        region = round([x1, y1, x2 - x1, y2 - y1]);
    else
        region = round([round(region(1)), round(region(2)), ...
            round(region(1) + region(3)) - round(region(1)), ...
            round(region(2) + region(4)) - round(region(2))]);
    end;

    x1 = max(1, region(1));
    y1 = max(1, region(2));
    x2 = min(width-2, region(1) + region(3) - 1);
    y2 = min(height-2, region(2) + region(4) - 1);

    template = gray((y1:y2)+1, (x1:x2)+1);

    state = struct('template', template, 'size', [x2 - x1 + 1, y2 - y1 + 1]);
    state.window = max(state.size) * 2;
    state.position = [x1 + x2 + 1, y1 + y2 + 1] / 2;

    location = [x1, y1, state.size];

end

function [state, mask, confidence] = ncc_update(state, I, varargin)

    confidence = 0;
    gray = double(rgb2gray(I)) ;

    [height, width] = size(gray);

    x1 = max(1, round(state.position(1) - state.window / 2));
    y1 = max(1, round(state.position(2) - state.window / 2));
    x2 = min(width-2, round(state.position(1) + state.window / 2));
    y2 = min(height-2, round(state.position(2) + state.window / 2));

    region = gray((y1:y2)+1, (x1:x2)+1);

    if any(size(region) < size(state.template))
        location = [state.position - state.size / 2, state.size];
        return;
    end;

    C = normxcorr2(state.template, region);

    % We are only using valid part of the response (where full template is used)
    pad = size(state.template) - 1;
    center = size(region) - pad - 1;
    C = C([false(1,pad(1)) true(1,center(1))], [false(1,pad(2)) true(1,center(2))]);

    x1 = x1 + pad(2);
    y1 = y1 + pad(1);
    [confidence, imax] = max(C(:));
    if isempty(imax)
        mx = 0;
        my = 0;
    else
        [my, mx] = ind2sub(size(C),imax(1));
    end

    position = [x1 + mx - state.size(1) / 2, y1 + my - state.size(2) / 2];

    state.position = position;
    location = [position - state.size / 2, state.size];
    
    mask = mask_from_rect(location, size(I, 2), size(I, 1));

end

function x = make_full_size(x, output_width, output_height)
    
    if size(x, 1) ~= output_height || size(x, 2) ~= output_width
        
        pad_x = output_width - size(x, 2);
        if pad_x < 0
            x = x(:, 1:size(x, 2) + pad_x);
            pad_x = 0;
        end
        
        pad_y = output_height - size(x, 1);
        if pad_y < 0
            x = x(:, 1:size(x, 1) + pad_y);
            pad_y = 0;
        end
        
        x = padarray(x, [pad_y, pad_x], 0, 'post');
        
    end

end

function rect = rect_from_mask(mask)
    x_ = sum(mask, 1);
    y_ = sum(mask, 2);
    x0 = min(find(x_ > 0));
    x1 = max(find(x_ > 0));
    y0 = min(find(y_ > 0));
    y1 = max(find(y_ > 0));
    rect = [x0, y0, x1 - x0 + 1, y1 - y0 + 1];
end

function mask = mask_from_rect(rect, mask_width, mask_height)
    mask = zeros(mask_height, mask_width);
    x0 = round(rect(1));
    y0 = round(rect(2));
    x1 = round(x0 + rect(3) - 1);
    y1 = round(y0 + rect(4) - 1);
    mask(y0:y1, x0:x1) = 1;
end
