function varargout = AAMVariation(varargin)
% AAMVARIATION M-file for AAMVariation.fig
%      AAMVARIATION, by itself, creates a new AAMVARIATION or raises the existing
%      singleton*.
%
%      H = AAMVARIATION returns the handle to a new AAMVARIATION or the handle to
%      the existing singleton*.
%
%      AAMVARIATION('CALLBACK',hObject,eventData,handles,...) calls the local
%      function named CALLBACK in AAMVARIATION.M with the given input arguments.
%
%      AAMVARIATION('Property','Value',...) creates a new AAMVARIATION or raises the
%      existing singleton*.  Starting from the left, property value pairs are
%      applied to the GUI before AAMVariation_OpeningFcn gets called.  An
%      unrecognized property name or invalid value makes property application
%      stop.  All inputs are passed to AAMVariation_OpeningFcn via varargin.
%
%      *See GUI Options on GUIDE's Tools menu.  Choose "GUI allows only one
%      instance to run (singleton)".
%
% See also: GUIDE, GUIDATA, GUIHANDLES

% Edit the above text to modify the response to help AAMVariation

% Last Modified by GUIDE v2.5 29-Sep-2010 15:52:16

% Begin initialization code - DO NOT EDIT
gui_Singleton = 1;
gui_State = struct('gui_Name',       mfilename, ...
                   'gui_Singleton',  gui_Singleton, ...
                   'gui_OpeningFcn', @AAMVariation_OpeningFcn, ...
                   'gui_OutputFcn',  @AAMVariation_OutputFcn, ...
                   'gui_LayoutFcn',  [] , ...
                   'gui_Callback',   []);
if nargin && ischar(varargin{1})
    gui_State.gui_Callback = str2func(varargin{1});
end

if nargout
    [varargout{1:nargout}] = gui_mainfcn(gui_State, varargin{:});
else
    gui_mainfcn(gui_State, varargin{:});
end
% End initialization code - DO NOT EDIT


% --- Executes just before AAMVariation is made visible.
function AAMVariation_OpeningFcn(hObject, eventdata, handles, varargin)
% This function has no output args, see OutputFcn.
% hObject    handle to figure
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
% varargin   command line arguments to AAMVariation (see VARARGIN)

% Choose default command line output for AAMVariation
handles.output = hObject;

handles.Model = struct;
handles.ShapeModel = struct;
handles.AppearanceModel = struct;

handles.ShapeSelected = 1;
handles.AppearanceSelected = 1;
handles.CombinedSelected = 1;

% Update handles structure
guidata(hObject, handles);


% UIWAIT makes AAMVariation wait for user response (see UIRESUME)
% uiwait(handles.figure1);


% --- Outputs from this function are returned to the command line.
function varargout = AAMVariation_OutputFcn(hObject, eventdata, handles) 
% varargout  cell array for returning output args (see VARARGOUT);
% hObject    handle to figure
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Get default command line output from handles structure
varargout{1} = handles.output;


% --- Executes on slider movement.
function shapeSlider_Callback(hObject, eventdata, handles)
% hObject    handle to shapeSlider (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)        
% Hints: get(hObject,'Value') returns position of slider
%        get(hObject,'Min') and get(hObject,'Max') to determine range of slider

    sliderVal = get(hObject,'Value');
    
    Model = handles.Model;
    ShapeModel = handles.ShapeModel;
    AppearanceModel = handles.AppearanceModel;
    
    params = zeros(size(ShapeModel.Variances));
    params(handles.ShapeSelected) = sqrt(ShapeModel.Variances(handles.ShapeSelected))*sliderVal;
    
    ImageToDrawOn = zeros([ AppearanceModel.TextureDimensions 3]);
       
    shapeTranslationFactor = max(ShapeModel.MeanShape(:)) - min(ShapeModel.MeanShape(:));

       % sx, sy, tx, ty
    T = [ (AppearanceModel.TextureDimensions(1) - 0.15 * AppearanceModel.TextureDimensions(1)) / shapeTranslationFactor, 0, AppearanceModel.TextureDimensions(1) / 2, AppearanceModel.TextureDimensions(2) / 2 ];
           
    %axes(handles.axes2);
    ImageToDrawOn = DrawTextureOnTop(ImageToDrawOn, Model, params, zeros(numel(AppearanceModel.Variances), 1), T, AppearanceModel.Transform.TranslateGlobal, AppearanceModel.Transform.ScaleGlobal); 
    imshow(ImageToDrawOn, 'Parent', handles.axes2);


% --- Executes during object creation, after setting all properties.
function shapeSlider_CreateFcn(hObject, eventdata, handles)
% hObject    handle to shapeSlider (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: slider controls usually have a light gray background.
if isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor',[.9 .9 .9]);
end
set(hObject, 'Value', 0);

% --- Executes on slider movement.
function appearanceSlider_Callback(hObject, eventdata, handles)
% hObject    handle to appearanceSlider (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: get(hObject,'Value') returns position of slider
%        get(hObject,'Min') and get(hObject,'Max') to determine range of slider

    sliderVal = get(hObject,'Value');
    
    Model = handles.Model;
    ShapeModel = handles.ShapeModel;
    AppearanceModel = handles.AppearanceModel;
    
    params = zeros(size(AppearanceModel.Variances));
    params(handles.AppearanceSelected) = sqrt(AppearanceModel.Variances(handles.AppearanceSelected))*sliderVal;
    
    ImageToDrawOn = zeros([ AppearanceModel.TextureDimensions 3]);
       
    shapeTranslationFactor = max(ShapeModel.MeanShape(:)) - min(ShapeModel.MeanShape(:));

       % sx, sy, tx, ty
    T = [ (AppearanceModel.TextureDimensions(1) - 0.15 * AppearanceModel.TextureDimensions(1)) / shapeTranslationFactor, 0, AppearanceModel.TextureDimensions(1) / 2, AppearanceModel.TextureDimensions(2) / 2 ];
           
    %axes(handles.axes2);
    ImageToDrawOn = DrawTextureOnTop(ImageToDrawOn, Model, zeros(numel(ShapeModel.Variances), 1), params, T, AppearanceModel.Transform.TranslateGlobal, AppearanceModel.Transform.ScaleGlobal); 
    imshow(ImageToDrawOn, 'Parent', handles.axes3);
    

% --- Executes during object creation, after setting all properties.
function appearanceSlider_CreateFcn(hObject, eventdata, handles)
% hObject    handle to appearanceSlider (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: slider controls usually have a light gray background.
if isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor',[.9 .9 .9]);
end
set(hObject, 'Value', 0);

% --- Executes on slider movement.
function combinedSlider_Callback(hObject, eventdata, handles)
% hObject    handle to combinedSlider (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: get(hObject,'Value') returns position of slider
%        get(hObject,'Min') and get(hObject,'Max') to determine range of slider


% --- Executes during object creation, after setting all properties.
function combinedSlider_CreateFcn(hObject, eventdata, handles)
% hObject    handle to combinedSlider (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: slider controls usually have a light gray background.
if isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor',[.9 .9 .9]);
end
set(hObject, 'Value', 0);

% --- Executes on selection change in shapePopupMenu.
function shapePopupMenu_Callback(hObject, eventdata, handles)
% hObject    handle to shapePopupMenu (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: contents = cellstr(get(hObject,'String')) returns shapePopupMenu contents as cell array
%        contents{get(hObject,'Value')} returns selected item from shapePopupMenu
    handles.ShapeSelected = get(hObject,'Value');
    guidata(hObject, handles);
    
% --- Executes during object creation, after setting all properties.
function shapePopupMenu_CreateFcn(hObject, eventdata, handles)
% hObject    handle to shapePopupMenu (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: popupmenu controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end

% --- Executes on selection change in appearancePopupMenu.
function appearancePopupMenu_Callback(hObject, eventdata, handles)
% hObject    handle to appearancePopupMenu (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: contents = cellstr(get(hObject,'String')) returns appearancePopupMenu contents as cell array
%        contents{get(hObject,'Value')} returns selected item from appearancePopupMenu
    handles.AppearanceSelected = get(hObject,'Value');
    guidata(hObject, handles);

% --- Executes during object creation, after setting all properties.
function appearancePopupMenu_CreateFcn(hObject, eventdata, handles)
% hObject    handle to appearancePopupMenu (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: popupmenu controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end


% --- Executes on selection change in popupmenu3.
function popupmenu3_Callback(hObject, eventdata, handles)
% hObject    handle to popupmenu3 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: contents = cellstr(get(hObject,'String')) returns popupmenu3 contents as cell array
%        contents{get(hObject,'Value')} returns selected item from popupmenu3


% --- Executes during object creation, after setting all properties.
function popupmenu3_CreateFcn(hObject, eventdata, handles)
% hObject    handle to popupmenu3 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: popupmenu controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end


% --- Executes on button press in showTriangulationMean.
function showTriangulationMean_Callback(hObject, eventdata, handles)
% hObject    handle to showTriangulationMean (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hint: get(hObject,'Value') returns toggle state of showTriangulationMean


% --- Executes on button press in checkbox3.
function checkbox3_Callback(hObject, eventdata, handles)
% hObject    handle to checkbox3 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hint: get(hObject,'Value') returns toggle state of checkbox3


% --- Executes on button press in checkbox4.
function checkbox4_Callback(hObject, eventdata, handles)
% hObject    handle to checkbox4 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hint: get(hObject,'Value') returns toggle state of checkbox4


% --- Executes on button press in checkbox5.
function checkbox5_Callback(hObject, eventdata, handles)
% hObject    handle to checkbox5 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hint: get(hObject,'Value') returns toggle state of checkbox5


% --- Executes on button press in pushbutton1.
function pushbutton1_Callback(hObject, eventdata, handles)
    LoadDependencies;

    [file, path] = uigetfile('*.mat');
    load([path file]);
    
    Model = StatisticalModel(1);
    AppearanceModel = Model.AppearanceModel;
    ShapeModel = Model.ShapeModel;
    
    ImageToDrawOn = zeros([ AppearanceModel.TextureDimensions 3]);
       
    shapeTranslationFactor = max(ShapeModel.MeanShape(:)) - min(ShapeModel.MeanShape(:));

       % sx, sy, tx, ty
    T = [ (AppearanceModel.TextureDimensions(1) - 0.15 * AppearanceModel.TextureDimensions(1)) / shapeTranslationFactor, 0, AppearanceModel.TextureDimensions(1) / 2, AppearanceModel.TextureDimensions(2) / 2 ];
           
    ImageToDrawOn = DrawTextureOnTop(ImageToDrawOn, Model, zeros(numel(ShapeModel.Variances), 1), zeros(numel(AppearanceModel.Variances), 1), T, AppearanceModel.Transform.TranslateGlobal, AppearanceModel.Transform.ScaleGlobal); 
    
    imshow(ImageToDrawOn, 'Parent', handles.axes1);    
    imshow(ImageToDrawOn, 'Parent', handles.axes2);
    imshow(ImageToDrawOn, 'Parent', handles.axes3);
    imshow(ImageToDrawOn, 'Parent', handles.axes4);
        
    labels = {1:numel(ShapeModel.Variances)};
    set(handles.shapePopupMenu,'String',labels);
    
    labels = {1:numel(AppearanceModel.Variances)};
    set(handles.appearancePopupMenu,'String',labels);    
    
    handles.Model = StatisticalModel(1);
    handles.AppearanceModel = Model.AppearanceModel;
    handles.ShapeModel = Model.ShapeModel;
    guidata(hObject, handles);

% hObject    handle to pushbutton1 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
