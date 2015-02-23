$(document).ready(function() {

    // Prevent the default navigation for the a.slide_button elements.
    // This is mostly here just for development, so feel free to comment out.
    //$('.slide_button').click(function(e){
    //    e.preventDefault();

    //    var $this = $(this),
    //        thisId = $this.attr('id');

    //    switch (thisId) {
    //        case 'slide_previous':
    //            slideBack($this);
    //            break;
    //        case 'slide_next':
    //            slideForward($this);
    //            break;
    //    }
    //});

    // Set focus to the first number (pops up the keyboard automatically on touch devices)
    $('.remote_code:first').focus();


    // This moves focus to the next field when the user enters a digit in each remote code field,
    // and then blurs it when they enter the final field.
    $('.remote_code').keyup(function(){
        console.log('meep');
        if (this.value.length == this.maxLength) {
            var $this = $(this),
                $next = $this.next('.remote_code');

            if ($next.length > 0) {
                $next.focus();
                console.log('moop');
            } else {
                $this.blur();
            }
        }

    });

    // trying to get gestures working.. blaaaarghhhhh!
    $('#container').swipe( {
        swipeLeft:function(event, direction, distance, duration, fingerCount, fingerData) {
            slideForward($(this));
        },
        swipeRight:function(event, direction, distance, duration, fingerCount, fingerData) {
            slideBack($(this)); 
        }
    });
});


function slideForward($this) {
    highlight($this);
    console.log('forward!');
}

function slideBack($this) {
    highlight($this);
    $('#event').text('back!');
}

function highlight($this) {
    $this.addClass('active');
}

function nextbuttonclick($this) {
        $.get("/raas/next", function(data) {
                console.log("next navigation success");
                console.log("data: ", data)
        });
}

function prevbuttonclick($this) {
        $.get("/raas/previous", function(data) {
                console.log("previous navigation success");
                console.log("data: ", data)
        });
}
