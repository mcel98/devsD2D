 
 //import DOMPurify from 'dompurify'
 
 function createWorkArea(){
    var graph = new joint.dia.Graph;
    graph.set('graphCustomProperty', true);
    graph.set('graphExportTime', Date.now());

    var paper = new joint.dia.Paper({
        el: document.getElementById('WorkArea'),
        model: graph,
        width: $('#WorkArea').width,
        height: $('#WorkArea').height,
        gridSize: 1,
        snapLinks: true,
        linkPinning: false,
        embeddingMode: true,
        defaultAnchor: { name: 'perpendicular' },
        defaultConnectionPoint: { name: 'boundary' },
        highlighting: {
            'default': {
                name: 'stroke',
                options: {
                    padding: 6
                }
            },
            'embedding': {
                name: 'addClass',
                options: {
                    className: 'highlighted-parent'
                }
            }
        },
    
        validateEmbedding: function(childView, parentView) {
            return parentView.model instanceof joint.shapes.devs.Coupled;
        },
    
        validateConnection: function(sourceView, sourceMagnet, targetView, targetMagnet) {
            return sourceMagnet != targetMagnet;
        }
    });

    //doble click genera el menu de submodulos del modulo
    paper.on('element:pointerdblclick', function(elementView) {
    

        attributesTool(elementView);

        
    });

    //eventos para eliminar una objeto

    paper.on('element:mouseenter', function(elementView) {
        elementView.showTools();
    });

    paper.on('element:mouseleave', function(elementView) {
        elementView.hideTools();
    });



    return paper;


 }

 //rect2.attr('label/text', 'World!');

//genero sub Modulo del modulo seleccionado
 function attributesTool(elementView){
    var currentElement = elementView.model
    // TODO: refactor. Pasar un archivo html para cada pincel para desacoplar la funcion de las propiedades del pincel y el html
    const elementMarker = '<a id="jmp-'+currentElement.id+'" class="list-group-item list-group-item-action" href="#'+ currentElement.id +'"> default </a>'

    const rename = '<h5>default</h5>' +
    '<div class="input-group">'+
        '<input type="text" name="quant[6]-'+currentElement.id+'" class="form-control input-name" value="default" min="0" max="10">'+
    '</div>';

    const inputs = '<h5>InPort</h5>' +
    '<div class="input-group">'+
        '<span class="input-group-btn">'+
            '<button type="button" id="in" class="btn btn-default btn-number-'+currentElement.id+'" data-type="minus" data-field="quant[1]">'+
                '<span class="glyphicon glyphicon-minus"></span>'+
            '</button>'+
        '</span>'+
        '<input type="text" name="quant[1]-'+currentElement.id+'" class="form-control input-number" value="0" min="0" max="10">'+
        '<span class="input-group-btn">'+
            '<button type="button" id="in" class="btn btn-default btn-number-'+currentElement.id+'" data-type="plus" data-field="quant[1]">'+
                '<span class="glyphicon glyphicon-plus"></span>'+
            '</button>'+
        '</span>'+
    '</div>';

    const outputs = '<h5>OutPort</h5>' +
    '<div class="input-group">'+
        '<span class="input-group-btn">'+
            '<button type="button" id="out" class="btn btn-default btn-number-'+currentElement.id+'"  data-type="minus" data-field="quant[2]">'+
                '<span class="glyphicon glyphicon-minus"></span>'+
            '</button>'+
        '</span>'+
        '<input type="text" name="quant[2]-'+currentElement.id+'" class="form-control input-number" value="0" min="0" max="10">'+
        '<span class="input-group-btn">'+
            '<button type="button" id="out" class="btn btn-default btn-number-'+currentElement.id+'" data-type="plus" data-field="quant[2]">'+
                '<span class="glyphicon glyphicon-plus"></span>'+
            '</button>'+
        '</span>'+
    '</div>';

    let size = currentElement.get('size');
    let width = size.width;
    let height = size.height;

    const resize = '<h5>Size</h5>' +
    '<div class="input-group">'+
        '<span class="input-group-btn">'+
            '<button type="button" id="out" class="btn btn-default btn-number-'+currentElement.id+'"  data-type="minus" data-field="quant[3]">'+
                '<span class="glyphicon glyphicon-minus"></span>'+
            '</button>'+
        '</span>'+
        '<input type="text" name="quant[3]-'+currentElement.id+'" class="form-control resize" value='+width+' min="1" max ="1000">'+
        '<span class="input-group-btn">'+
            '<button type="button" id="out" class="btn btn-default btn-number-'+currentElement.id+'" data-type="plus" data-field="quant[3]">'+
                '<span class="glyphicon glyphicon-plus"></span>'+
            '</button>'+
        '</span>'+
    '</div>'+
    '<div class="input-group">'+
        '<span class="input-group-btn">'+
            '<button type="button"  class="btn btn-default btn-number-'+currentElement.id+'"  data-type="minus" data-field="quant[4]">'+
                '<span class="glyphicon glyphicon-minus"></span>'+
            '</button>'+
        '</span>'+
        '<input type="text" name="quant[4]-'+currentElement.id+'" class="form-control resize" value="'+height+'" min="1" max="1000">'+
        '<span class="input-group-btn">'+
            '<button type="button"  class="btn btn-default btn-number-'+currentElement.id+'" data-type="plus" data-field="quant[4]">'+
                '<span class="glyphicon glyphicon-plus"></span>'+
            '</button>'+
        '</span>'+
    '</div>';

    let propertiesElement = '<div class= "border-bottom" id="'+currentElement.id+'" style="overflow-y: scroll; ; position:relative;" >' + rename + inputs + outputs + resize + '</div>' ;

  //funfact: insertAdjacentHTML es el mejor optimizado para concatenar html
    document.getElementById('shape-content').insertAdjacentHTML('beforeend', propertiesElement);
    document.getElementById('list-shape').insertAdjacentHTML('beforeend', elementMarker);

    //binding de eventos de boton
    $('.btn-number-'+ currentElement.id).click(function(e) {
        e.preventDefault();
        let fieldName = this.getAttribute('data-field');
        let type =  this.getAttribute('data-type');
        var input = $("input[name='"+fieldName+'-'+currentElement.id+"']");
        let currentVal = parseInt(input.val());
        let updatedSize = currentElement.get('size');
        let updatedWidth = updatedSize.width;
        let updatedHeight = updatedSize.height;
        console.log(updatedSize)

    
        if(!isNaN(currentVal)){
            if(type == 'minus'){
                if(currentVal > input.attr('min')) {
                    input.val(currentVal - 1).change();
    
                    if(fieldName == 'quant[2]'){
                        currentElement.removeOutPort('out'+input.val());
                    }else if(fieldName == 'quant[1]'){
                        currentElement.removeInPort('in'+input.val());
                    }else if(fieldName == 'quant[3]'){
                        currentElement.resize(updatedWidth -1, updatedHeight);
                    }else{
                        currentElement.resize(updatedWidth , updatedHeight -1);
                    }
                   
                    
    
                } 
                if(parseInt(input.val()) == input.attr('min')) {
                    $(this).attr('disabled', true);
                }
            } else if(type == 'plus') {
    
                if(currentVal < input.attr('max')) {
    
                    if(fieldName == 'quant[2]'){
                        currentElement.addOutPort('out'+input.val());
                    }else if(fieldName == 'quant[1]'){
                        currentElement.addInPort('in'+input.val());
                    }else if(fieldName == 'quant[3]'){
                        currentElement.resize(height+1, width);
                    }else{
                        currentElement.resize(height, width+1);
                    }
                    
                    input.val(currentVal + 1).change();
                }
                if(parseInt(input.val()) == input.attr('max')) {
                    $(this).attr('disabled', true);
                }
    
            }
        } else {
            input.val(0);
        }
        
      });
    
    
     $('.input-number').change(function() {
        
        let minValue =  parseInt($(this).attr('min'));
        let maxValue =  parseInt($(this).attr('max'));
        let valueCurrent = parseInt($(this).val());
        
        let name = $(this).attr('name');
        if(valueCurrent >= minValue) {
            $(".btn-number[data-type='minus'][data-field='"+name+"']").removeAttr('disabled');
        } else {
            alert('Sorry, the minimum value was reached');
            $(this).val($(this).data('oldValue'));
        }
        if(valueCurrent <= maxValue) {
            $(".btn-number[data-type='plus'][data-field='"+name+"']").removeAttr('disabled');
        } else {
            alert('Sorry, the maximum value was reached');
            $(this).val($(this).data('oldValue'));
        }
        
        
    });
    //binding de eventos de teclado
     $(".input-number").keydown(function (e) {
        e.preventDefault();  
    });

    $(".resize").keypress(function (e) {
        let fieldName = this.getAttribute('name');


       //backspace, delete, tab, escape, enter and .
       if ($.inArray(e.keyCode, [46, 8, 9, 27, 13, 190]) !== -1 ||
       //Ctrl+A
      (e.keyCode == 65 && e.ctrlKey === true) || 
       //home, end, left, right
      (e.keyCode >= 35 && e.keyCode <= 39)) {
           
           if(e.keyCode == 13){
            let value = $(this).val();
            let currentVal = parseInt(value);
            let updatedSize = currentElement.get('size');

            let updatedWidth = updatedSize.width;
            let updatedHeight = updatedSize.height;
            console.log(updatedSize)
               if(fieldName.startsWith('quant[3]')){
                currentElement.resize(currentVal, updatedHeight);    
               }else{
                currentElement.resize(updatedWidth , currentVal); 
               }
           }
        return
  }
  //chequear que sea un numero
        if ((e.shiftKey || (e.keyCode < 48 || e.keyCode > 57)) && (e.keyCode < 96 || e.keyCode > 105)) {
            e.preventDefault();
        }  
    });


    $(".input-name").keypress(function (e) {

       //backspace, delete, tab, escape, enter and .
       if ($.inArray(e.keyCode, [46, 8, 9, 27, 13, 190]) !== -1 ||
       //Ctrl+A
      (e.keyCode == 65 && e.ctrlKey === true) || 
       //home, end, left, right
      (e.keyCode >= 35 && e.keyCode <= 39)) {
           
           if(e.keyCode == 13){
            let currentVal = $(this).val();
            document.getElementById('jmp-'+currentElement.id).textContent = currentVal
            console.log(currentVal)
            currentElement.attr('label/text', currentVal);
            currentElement.attr('.label/text', currentVal);
            console.log(currentElement.attr())
           }
        return
  }
  //chequear que sea un numero

        if ((e.shiftKey || (e.keyCode <= 65 || e.keyCode >= 20)) && (e.keyCode == 32 || e.keyCode == 0)) {
            e.preventDefault();
        }  
    });
   

 }





 /**
  * *******************************************
  *                 PINCELES                  *
  * *******************************************
  * 
  */


 const defineTooling = () =>{

    let boundaryTool = new joint.elementTools.Boundary({
    padding: 20,
    rotate: true,
    useModelGeometry: true,
    });

    let removeButton = new joint.elementTools.Remove();

    let toolsView = new joint.dia.ToolsView({
    tools: [
        boundaryTool,
        removeButton
    ]
    });
    return toolsView
};


//Pincel de dibujo de coupled diagram
const drawCoupled = (event, paper) => {

    if( 'svg' == event.target.tagName){

        let x = event.clientX - WorkArea.offsetLeft
        let y = event.clientY - WorkArea.offsetTop;

        var devs = joint.shapes.devs;
        let graph = paper.model;
        var coupled = new devs.Coupled();
        coupled.position(x,y);
        coupled.size(300,300);

        graph.addCells(coupled);
        //despues de agregar elemento definir tooling
        let toolsView = defineTooling();
        var elementView = coupled.findView(paper);
        elementView.addTools(toolsView);
    }
        
};


//Pincel de dibujo de atomic
const drawAtomic = (event, paper) => {

    if( 'svg' == event.target.tagName){
        let x = event.clientX - WorkArea.offsetLeft
        let y = event.clientY - WorkArea.offsetTop;

        var devs = joint.shapes.devs;
        let graph = paper.model;
        var atomic = new devs.Atomic();
        atomic.position(x,y);
        atomic.size(100,100);

        graph.addCells(atomic);
        //despues de agregar elemento definir tooling <--- refactor
        let toolsView = defineTooling();
        var elementView = atomic.findView(paper);
        elementView.addTools(toolsView);
    }
        
};


const getCellId = (Eltarget) => {
    console.log(Eltarget);
    if(/^j_/.test(Eltarget.id)){
        return Eltarget.getAttribute('model-id');
    }else{

        let parent = Eltarget.parentElement;
        return getCellId(parent);
    }
    
};
/**
 * ****************************************************************
 *  EVENT HANDLING DE LOS DIFERENTES PARTES DEL GRAFO DEL EDITOR  *
 * ****************************************************************
 */ 


// todo: error handling -> mantener brush actual
const Brushes = (map, button, args) => {return map[button](...args);  };


const setBrush = (button, args) =>{
    
    return Brushes({'default':drawCoupled, 'place': drawCoupled, 'atomic': drawAtomic},button,args);

}

const WorkArea = document.getElementById('WorkArea');

//interaccion entre dos elementos con herramienta seleccionada

const interaction = (count, paper, brushType, cellsArray =[]) =>{

    WorkArea.onmousedown = event => {
        // count == cantidad de clicks
        // si el target no es el papel -> es un elemento del grafo
        if(event.target.tagName == 'svg'){
            event.stopPropagation();//detecto drags y evito la ejecucion del event
        }else{
            let newArray = cellsArray;
            if(count == 0){
                //agrego cell
                let id = getCellId(event.target);
                newArray.push(id)
                return interaction(count+1,paper,brushType,newArray);

            }else{
                
                //si ya tengo el primero elemento espero al segundo click
                let id = getCellId(event.target);
                newArray.push(id);
                setBrush(brushType,[newArray,event, paper]);
                return interaction(0,paper,brushType,[]);
            }
        }

    };
    console.log('end call');
    return;
};


//render del elemento seleccionado de la red de petri al cliquear sobre el papel

const render = ( count, paper, brushType,oneClickTimer = 0) =>{
    
    
    console.log('call');
    WorkArea.onmousedown = event => {
        // count == cantidad de clicks en 300 ms
        if(event.target.tagName != 'svg'){
            event.stopPropagation();//detecto drags y evito la ejecucion del evento
        }else{
            
            if(count == 0){
                
                //si se produce un click espero 300ms para el doble click
                var timeout = setTimeout(function(){
                
                    setBrush(brushType,[event, paper]);
                    
                    //Problema calls a render empiezan a crecer->tail recursion talvez?. Edit:creo que lo arregle... enfasis en el creo
                    return render(0,paper,brushType);
                    
                },300);
            
                //espero el doble click
                return render(count + 1,paper,brushType,timeout);

            }else{
                //se produce el doble click cancelo el renderizado
                clearTimeout(oneClickTimer);
                console.log('doble click');
                //al producirse el doble click reseteo el contador
                return render(0,paper,brushType,oneClickTimer);
            }
        }

    };
    console.log('endcall');
    return;
};

/**
 * ****************************************
 *          MANEJO DE TOOLBAR             *
 * ****************************************
 */



const toolType = (paper,type) => {
    const types = {'0': render, '1': interaction};
    const context = type.split(/([0-9]+)/);
    console.log(context);
    types[context[1]](0,paper,context[0]);


}

const toolBar = document.getElementById('toolBar');

//eleccion del brush en barra de herramientas

const toolSelection = (paper) =>{

    //al clickear sobre un boton de brush, llamamos a la funcion del brush dado 
    toolBar.addEventListener("click", function(event){
        
        if (event.target.id === 'tool') {
            const clean = event.target.name;//->riesgo de xss
 
            return toolType(paper,clean);
        }


    });

}

//inicializamos el editor
toolSelection(createWorkArea());

